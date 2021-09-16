#include <chrono>
#include <iostream>
#include <execution>
#include <array>

#include <PGE/File/BinaryWriter.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace PGE;

template<> void BinaryWriter::write<aiVector3D>(const aiVector3D& v) {
    write<float>(v.x); write<float>(v.y); write<float>(v.z);
}

static const FilePath converterResult(const FilePath& path) {
    return path.trimExtension() + ".b";
}

static int defaultSteps =
    aiProcess_CalcTangentSpace |
    aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate |
    aiProcess_GenSmoothNormals |
    aiProcess_PreTransformVertices |
    aiProcess_ImproveCacheLocality |
    aiProcess_FindDegenerates |
    aiProcess_FindInvalidData |
    aiProcess_GenUVCoords |
    aiProcess_TransformUVCoords |
    aiProcess_OptimizeMeshes;

struct ConnectionPoint {
    String mat;
    aiVector3D pos;
};

// Fuck you assimp, once again, this time for not providing constexpr vectors!!
static const std::array matNames {
    ConnectionPoint{ "ceiling_mat", aiVector3D() },
    ConnectionPoint{ "floor_mat", aiVector3D(7.f, -3.92830f, -5.38239f) },
    ConnectionPoint{ "innerwall_mat", aiVector3D() },
    ConnectionPoint{ "outerwall_mat", aiVector3D() }
};

// extra is called once for every vertex
static void writeMesh(BinaryWriter& writer, const aiScene* scene, int matIndex, const std::function<void(aiMesh*, unsigned)>& extra = [](auto...){ }) {
    u32 vertCount = 0;
    u32 indexCount = 0;
    for (unsigned i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        if (mesh->mMaterialIndex == matIndex) {
            vertCount += mesh->mNumVertices;
            indexCount += mesh->mNumFaces * 3;
        }
    }

    writer.write<u32>(vertCount);
    for (unsigned i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        if (mesh->mMaterialIndex == matIndex) {
            for (unsigned k = 0; k < mesh->mNumVertices; k++) {
                extra(mesh, k);
                writer.write<float>(mesh->mVertices[k].x * 100.f);
                writer.write<float>(mesh->mVertices[k].y * 100.f);
                writer.write<float>(mesh->mVertices[k].z * 100.f);
                writer.write(mesh->mNormals[k]);
                writer.write(mesh->mTangents[k]);
                writer.write(mesh->mBitangents[k]);
                aiVector3D uv = mesh->mTextureCoords[0][k]; // Lmao wtf ass imp
                writer.write<float>(uv.x); writer.write<float>(uv.y);
            }
        }
    }

    u32 totalIndex = 0;
    writer.write<u32>(indexCount);
    for (unsigned i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        if (mesh->mMaterialIndex == matIndex) {
            for (unsigned j = 0; j < mesh->mNumFaces; j++) {
                aiFace* face = &mesh->mFaces[j];
                PGE_ASSERT(face->mNumIndices == 3, "Non-triangular face!");
                for (int k = 3 - 1; k >= 0; k--) {
                    writer.write<u32>(totalIndex + face->mIndices[k]);
                }
            }
            totalIndex += mesh->mNumVertices;
        }
    }
}

static bool convertModel(const FilePath& file) {
    Assimp::Importer importer;
    if (!importer.IsExtensionSupported(("." + file.getExtension()).cstr())) {
        return false;
    }

    const aiScene* scene = importer.ReadFile(file.str().cstr(), defaultSteps);
    PGE_ASSERT(scene != nullptr, importer.GetErrorString());

    BinaryWriter writer(converterResult(file));

    unsigned j = 0;
    for (unsigned i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* mat = scene->mMaterials[i];
        String name = mat->GetName().C_Str();
        std::cout << name << std::endl;
        if (name == matNames[j].mat) {
            aiMesh* closestMesh = nullptr;
            unsigned closestIndex;
            ai_real shortestLength = std::numeric_limits<ai_real>::infinity();
            writeMesh(writer, scene, i, [&](aiMesh* mesh, unsigned index) {
                ai_real dist = (mesh->mVertices[index] - matNames[j].pos).SquareLength();
                if (dist < shortestLength) {
                    closestMesh = mesh;
                    closestIndex = index;
                    shortestLength = dist;
                }
            });
            //PGE_ASSERT(Math::equalFloats(0.f, shortestLength), "Could not find vertex at exit for material " + name);
            aiVector3D uv = closestMesh->mTextureCoords[0][closestIndex];
            writer.write<float>(uv.x); writer.write<float>(uv.y);
            std::cout << sqrt(shortestLength) << " " << uv.x << ", " << uv.y << ", " << uv.z << std::endl;

            j++;
        }
    }
    PGE_ASSERT(matNames.size() == j, "Not all connection meshes were included!");

    j = 0;
    writer.write<byte>(scene->mNumMaterials - 4);
    for (unsigned i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* mat = scene->mMaterials[i];

        bool contains = false;
        for (int k = j; k < matNames.size(); k++) {
            if (mat->GetName().C_Str() == matNames[k].mat) {
                j = k; contains = true; break;
            }
        }
        if (contains) { continue; }

        aiString texName;
        PGE_ASSERT(mat->GetTexture(aiTextureType_DIFFUSE, 0, &texName) == aiReturn_SUCCESS, "Failed to get texture name");
        String tex(texName.C_Str());
        tex = tex.replace("\\", "/");
        String write = tex.substr(tex.findLast("/") - 1, tex.findLast("."));
        writer.write(tex.substr(tex.findLast("/") - 1, tex.findLast(".")));

        writeMesh(writer, scene, i);
        std::cout << mat->GetName().C_Str() << std::endl;
    }

    return true;
}

int main(int argc, char** argv) {
    String file;
    if (argc == 2) {
        file = argv[1];
    } else {
        std::cout << "Model: ";
        std::cin >> file;
    }

    using Clock = std::chrono::steady_clock;
    Clock::time_point start(Clock::now());

    if (*file.begin() == '"' && *file.rbegin() == '"') {
        file = file.substr(file.begin() + 1, file.rbegin());
    }

    bool recompile = false;
#if 0 // Set to 1 when working on this.
    recompile = true;
#endif

    FilePath path = FilePath::fromStr(file);
    if (path.isDirectory()) {
        std::vector<FilePath> paths = path.enumerateFiles();

        std::for_each(std::execution::par_unseq, paths.begin(), paths.end(), [=](const FilePath& modelPath) {
            FilePath toPath = converterResult(modelPath);
            if (!recompile && (toPath.exists() && toPath.getLastModifyTime() >= modelPath.getLastModifyTime())) {
                std::cout << "Already converted \"" + modelPath.str() + "\"!\n";
            } else {
                std::cout << "Started converting \"" + modelPath.str() + "\"!\n";
                if (convertModel(modelPath)) {
                    std::cout << "Finished converting \"" + toPath.str() + "\"!\n";
                } else {
                    std::cout << "Extension \"" + modelPath.getExtension() + "\" is not supported!\n";
                }
            }
        });
    } else {
        std::cout << "Converting only file: \"" << file << "\"\n";
        convertModel(path);
    }

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count() << "ms elapsed\n";
}
