#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/renderobject.h"

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual start() final;
    void virtual update(double time) final;

private:
    moar::RenderObject* monkey1;
    moar::RenderObject* monkey2;
    moar::RenderObject* monkey3;
    moar::RenderObject* torus1;
    moar::RenderObject* torus2;
    moar::RenderObject* torus3;
};

#endif // MYAPP_H
