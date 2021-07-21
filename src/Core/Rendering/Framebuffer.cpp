#include "Framebuffer.h"

#include "Outrospection.h"
#include "Util.h"

Framebuffer::Framebuffer(int width, int height) : isDefaultFramebuffer(false),
    defaultResolution(width, height), resolution(width, height)
{
    scaleResolution(glm::vec2(1.0));
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, resolution.x, resolution.y);

    auto& o = Outrospection::get();
    
    o.curFbResolution = &resolution;

    const glm::mat4 projection = glm::ortho(0.0f, float(resolution.x), float(resolution.y),
                                            0.0f, -1.0f, 1.0f);
    o.spriteShader.use();
    o.spriteShader.setMat4("projection", projection);

    o.glyphShader.use();
    o.glyphShader.setMat4("projection", projection);
}

void Framebuffer::bindTexture()
{
    glBindTexture(GL_TEXTURE_2D, texId);
}

void Framebuffer::scaleResolution(const glm::vec2& scale)
{
    resolution.x = float(defaultResolution.x) * scale.x;
    resolution.y = float(defaultResolution.y) * scale.y;

    // only update texture and stuff if it's a custom fb
    if(isDefaultFramebuffer)
        return;
    
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    // create a color attachment texture
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Framebuffer is not complete!");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
