/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "../include/core/SkCanvas.h"
#include "../include/core/SkTypeface.h"
#include "../include/core/SkStream.h"
#include "../include/core/SkData.h"
#include "../include/core/SkSurface.h"
#include "../include/core/SkRefCnt.h"
#include "../include/effects/SkGradientShader.h"
#include "../include/gpu/GrContext.h"
#include "../include/gpu/gl/GrGLInterface.h"
//#include "../include/gpu/gl/GrGLDefines.h"
#include "../include/core/SkPictureRecorder.h"
#include "../tools/Resources.h"
#include "yaml.h"

// These headers are just handy for writing this example file.  Nothing Skia specific.
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <assert.h>

#include <chrono>

#include <GLFW/glfw3.h>

static int gWidth = 1920;
static int gHeight = 1080;

extern void drawYAMLFile(SkCanvas *canvas, const char *file);

// These setup_gl_context() are not meant to represent good form.
// They are just quick hacks to get us going.
#if defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
    static bool setup_gl_context() {
        CGLPixelFormatAttribute attributes[] = { (CGLPixelFormatAttribute)0 };
        CGLPixelFormatObj format;
        GLint npix;
        CGLChoosePixelFormat(attributes, &format, &npix);
        CGLContextObj context;
        CGLCreateContext(format, nullptr, &context);
        CGLSetCurrentContext(context);
        CGLReleasePixelFormat(format);
        return true;
    }
#else
    static bool setup_gl_context() {
        return false;
    }
#endif

// Most pointers returned by Skia are derived from SkRefCnt,
// meaning we need to call ->unref() on them when done rather than delete them.
template <typename T> std::shared_ptr<T> adopt(T* ptr) {
    return std::shared_ptr<T>(ptr, [](T* p) { p->unref(); });
}

static std::shared_ptr<SkSurface> create_raster_surface(int w, int h) {
    std::cout << "Using raster surface" << std::endl;
    return adopt(SkSurface::MakeRasterN32Premul(w, h).release());
}

static std::shared_ptr<SkSurface> create_opengl_surface(int w, int h) {
    std::cout << "Using opengl surface" << std::endl;
    std::shared_ptr<GrContext> grContext = adopt(GrContext::Create(kOpenGL_GrBackend, 0));
    return adopt(SkSurface::MakeRenderTarget(grContext.get(),
                                            SkBudgeted::kNo,
                                            SkImageInfo::MakeN32Premul(w,h)).release());
}


sk_sp<SkImage> GetResourceAsImage(const char* path) {
    sk_sp<SkData> resourceData(SkData::MakeFromFileName(path));
    return SkImage::MakeFromEncoded(resourceData);
}


extern SkImageEncoder_EncodeReg gEReg;
SkImageEncoder_EncodeReg *k = &gEReg;


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}



sk_sp<SkPicture> gPic;
void draw(void) {
}

int main(int argc, char** argv) {
    
    SkPictureRecorder recorder;
    SkCanvas* skp_canvas = recorder.beginRecording(gWidth, gHeight, nullptr, 0);

    static const char* msg = "Hello world!";
    skp_canvas->clear(SK_ColorRED);
    //canvas->drawText(msg, strlen(msg), 90,120, paint);

    drawYAMLFile(skp_canvas, argv[1]);

    sk_sp<SkPicture> pic = recorder.finishRecordingAsPicture();
    // Draw to the surface via its SkCanvas.
    gPic = pic;

#if 0
    SkFILEWStream stream("out.skp");
    pic->serialize(&stream);
#endif

#if 0
    std::shared_ptr<SkSurface> surface = create_raster_surface(gWidth, gHeight);
    SkCanvas* ic = surface->getCanvas();   // We don't manage this pointer's lifetime.
    pic->playback(ic);

    // Grab a snapshot of the surface as an immutable SkImage.
    sk_sp<SkImage> image = surface->makeImageSnapshot();
    // Encode that image as a .png into a blob in memory.
    std::shared_ptr<SkData> png = adopt(image->encode(SkImageEncoder::kPNG_Type, 100));

    // This code is no longer Skia-specific.  We just dump the .png to disk.  Any way works.
    static const char* path = "example.png";
    std::ofstream(path, std::ios::out | std::ios::binary)
        .write((const char*)png->data(), png->size());
    std::cout << "Wrote " << path << std::endl;
#endif

    std::cout << "Rendering..." << std::endl;


    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(gWidth, gHeight, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    sk_sp<const GrGLInterface> fBackendContext;

    sk_sp<const GrGLInterface> glInterface;
    glInterface.reset(GrGLCreateNativeInterface());
    fBackendContext.reset(GrGLInterfaceRemoveNVPR(glInterface.get()));

    //SkASSERT(nullptr == fContext);
    auto fContext = GrContext::Create(kOpenGL_GrBackend, (GrBackendContext)fBackendContext.get());

    sk_sp<SkSurface> fSurface;
    if (nullptr == fSurface) {
        auto fActualColorBits =  24;

        if (fContext) {
            GrBackendRenderTargetDesc desc;
            desc.fWidth = gWidth;
            desc.fHeight = gHeight;
            desc.fConfig = kRGBA_8888_GrPixelConfig;
            desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
            desc.fSampleCnt = 0;
            desc.fStencilBits = 0;
            GrGLint buffer;

#ifndef GL_FRAMEBUFFER_BINDING
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#endif

            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);
            desc.fRenderTargetHandle = buffer;
            SkSurfaceProps    fSurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType);

            fSurface = SkSurface::MakeFromBackendRenderTarget(fContext, desc, &fSurfaceProps);

        }
    }

    glClearColor(0.0f,1.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SkCanvas* canvas = fSurface->getCanvas();   // We don't manage this pointer's lifetime.

    using FpMilliseconds = 
        std::chrono::duration<double, std::chrono::milliseconds::period>;


    auto before = std::chrono::high_resolution_clock::now();
    auto first = std::chrono::high_resolution_clock::now();

    auto min_frame = FpMilliseconds::max();
    auto min_min_frame = FpMilliseconds::max();
    auto max_frame = FpMilliseconds::min();
    auto max_max_frame = FpMilliseconds::min();
    auto sum_frame = FpMilliseconds::zero();

    // I really would like some of binning of series data here
    double sum_block_avg_ms = 0.;
    uint32_t num_ms_recorded = 0;

    const uint32_t frames_between_dumps = 60;
    const uint32_t exit_after_seconds = 3;

    uint32_t frame = 0;
    bool warmed_up = false;

    while (!glfwWindowShouldClose(window))
    {
        canvas->drawPicture(gPic);
        canvas->flush();
        glfwSwapBuffers(window);
        glfwPollEvents();

        warmed_up = frame > frames_between_dumps;
        auto after = std::chrono::high_resolution_clock::now();
        auto dur = after - before;

        min_frame = std::min(min_frame, FpMilliseconds(dur));
        max_frame = std::max(max_frame, FpMilliseconds(dur));
        sum_frame += dur;

        // only count globals for warmed up frames
        if (warmed_up) {
            min_min_frame = std::min(min_min_frame, FpMilliseconds(dur));
            max_max_frame = std::max(max_max_frame, FpMilliseconds(dur));
        }

        if ((++frame % frames_between_dumps) == 0) {
            double ms = (sum_frame / frames_between_dumps).count();
            printf("%3.3f [%3.3f .. %3.3f]  -- %4.2f fps",
                   ms, min_frame.count(), max_frame.count(), 1000.0 / ms);
            if (warmed_up) {
                printf("  -- (global %3.3f .. %3.3f)\n", min_min_frame.count(), max_max_frame.count());
                sum_block_avg_ms += ms;
                num_ms_recorded++;
            } else {
                printf("\n");
            }

            min_frame = FpMilliseconds::max();
            max_frame = FpMilliseconds::min();
            sum_frame = FpMilliseconds::zero();
        }

        //printf("%f\n", FpMilliseconds(after - before).count());
        before = after;

        if ((after-first) > std::chrono::seconds(exit_after_seconds)) {
            double average_ms = sum_block_avg_ms / double(num_ms_recorded);
            printf("min, avg, max (ms ):  % -4.3f, % -4.3f, % -4.3f\n",
                   min_min_frame.count(), average_ms, max_max_frame.count());
            printf("              (fps):  % -4.3f, % -4.3f, % -4.3f\n",
                   1000.0 / min_min_frame.count(), 1000.0 / average_ms, 1000.0 / max_max_frame.count());
            exit(0);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

