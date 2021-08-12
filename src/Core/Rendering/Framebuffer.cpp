#include "Framebuffer.h"

#include "Outrospection.h"
#include "Util.h"

Framebuffer::Framebuffer(int width, int height) : isDefaultFramebuffer(false),
    defaultResolution(width, height), resolution(width, height)
{
    scaleResolution(1.0);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);


    auto& o = Outrospection::get();
    if (isDefaultFramebuffer) // default fb letterboxing
    {
        glm::ivec2 windowRes = o.getWindowResolution();
        float targetAspectRatio = 1920 / 1080.f;

        int width = windowRes.x;
        int height = int(width / targetAspectRatio + 0.5f);

        if (height > windowRes.y)
        {
            // it doesn't fit our height, we must switch to pillarbox
            height = windowRes.y;
            width = (int)(height * targetAspectRatio + 0.5f);
        }

        // centered viewport in the backbuffer
        int vp_x = (windowRes.x / 2) - (width / 2);
        int vp_y = (windowRes.y / 2) - (height / 2);

        // OpenGL considers 0,0 the lower left corner of the screen
        glViewport(vp_x, windowRes.y - (vp_y + height), width, height);

    } else
    {
        glViewport(0, 0, resolution.x, resolution.y);
    }

    
    o.curFbResolution = &resolution;

    const glm::mat4 projection = glm::ortho(0.0f, float(resolution.x), float(resolution.y),
                                            0.0f, -1.0f, 1.0f);
    o.spriteShader.use();
    o.spriteShader.setMat4("projection", projection);

    o.inkShader.use();
    o.inkShader.setMat4("projection", projection);
    o.inkShader.setInt("viewportHeight", resolution.y);

    o.glyphShader.use();
    o.glyphShader.setMat4("projection", projection);
}

void Framebuffer::bindTexture()
{
    glBindTexture(GL_TEXTURE_2D, texId);
}

void Framebuffer::scaleResolution(float scale)
{
    resolution.x = float(defaultResolution.x) * scale;
    resolution.y = float(defaultResolution.y) * scale;

    // only update texture and stuff if it's a custom fb
    if(isDefaultFramebuffer)
        return;
    
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // create color attachment texture
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    
    // create RBO
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Framebuffer is not complete!");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
