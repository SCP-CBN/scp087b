#ifndef B_LIGHT_H_INCLUDED
#define B_LIGHT_H_INCLUDED

#include <PGE/Graphics/Shader.h>

using namespace PGE;

class Light {
    public:
        Light(Shader::Constant& position, Shader::Constant& rotation, Shader::Constant& intensity, Shader::Constant& range) :
            lightPos(position), viewRot(rotation), intensity(intensity), range(range) {};
        void setPos(Vector3f pos) { lightPos.setValue(pos); };
        void setView(Vector3f viewpos) { viewRot.setValue(viewpos); };
        void setRange(float newRange) { range.setValue(newRange); };
        void setIntensity(Vector3f intens) { intensity.setValue(intens); };

    private:
        Shader::Constant& lightPos;
        Shader::Constant& viewRot;
        Shader::Constant& intensity;
        Shader::Constant& range;
};

#endif // B_LIGHT_H_INCLUDED
