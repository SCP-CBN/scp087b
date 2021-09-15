#include <chrono>
#include <iostream>
#include <execution>

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

static bool convertModel(const FilePath& file) {
    Assimp::Importer importer;
    if (!importer.IsExtensionSupported(("." + file.getExtension()).cstr())) {
        return false;
    }

    const aiScene* scene = importer.ReadFile(file.str().cstr(),
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_FindDegenerates |
        aiProcess_FindInvalidData |
        aiProcess_GenUVCoords |
        aiProcess_TransformUVCoords |
        aiProcess_OptimizeMeshes
    );

    PGE_ASSERT(scene != nullptr, importer.GetErrorString());

    BinaryWriter writer(converterResult(file));
    writer.write<byte>(scene->mNumMaterials);
    for (unsigned i = 0; i < scene->mNumMaterials; i++) {
        aiString texName;
        PGE_ASSERT(scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texName) == aiReturn_SUCCESS, "Failed to get texture name");
        String tex(texName.C_Str());
        tex = tex.replace("\\", "/");
        String write = tex.substr(tex.findLast("/") - 1, tex.findLast("."));
        writer.write(tex.substr(tex.findLast("/") - 1, tex.findLast(".")));

        u32 vertCount = 0;
        u32 indexCount = 0;
        for (unsigned j = 0; j < scene->mNumMeshes; j++) {
            aiMesh* mesh = scene->mMeshes[j];
            if (mesh->mMaterialIndex == i) {
                vertCount += mesh->mNumVertices;
                indexCount += mesh->mNumFaces * 3;
            }
        }

        writer.write<u32>(vertCount);
        for (unsigned j = 0; j < scene->mNumMeshes; j++) {
            aiMesh* mesh = scene->mMeshes[j];
            if (mesh->mMaterialIndex == i) {
                for (unsigned k = 0; k < mesh->mNumVertices; k++) {
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
        for (unsigned j = 0; j < scene->mNumMeshes; j++) {
            aiMesh* mesh = scene->mMeshes[j];
            if (mesh->mMaterialIndex == i) {
                for (unsigned k = 0; k < mesh->mNumFaces; k++) {
                    aiFace* face = &mesh->mFaces[k];
                    PGE_ASSERT(face->mNumIndices == 3, "Non-triangular face!");
                    for (int l = 3 - 1; l >= 0; l--) {
                        writer.write<u32>(totalIndex + face->mIndices[l]);
                    }
                }
                totalIndex += mesh->mNumVertices;
            }
        }
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
