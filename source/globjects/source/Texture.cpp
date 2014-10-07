#include <globjects/Texture.h>

#include <algorithm>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/boolean.h>

#include <glm/gtc/type_ptr.hpp>

#include <globjects/Buffer.h>
#include <globjects/ObjectVisitor.h>

#include "pixelformat.h"
#include "Resource.h"


using namespace gl;

namespace globjects
{

Texture::Texture()
: Texture(GL_TEXTURE_2D)
{
}

Texture::Texture(GLenum  target)
: Object(new TextureResource)
, m_target(target)
{
}

Texture::Texture(IDResource * resource, GLenum  target)
: Object(resource)
, m_target(target)
{
}

Texture * Texture::fromId(GLuint id, GLenum target)
{
    return new Texture(new ExternalResource(id), target);
}


Texture::~Texture()
{
}

Texture * Texture::createDefault()
{
    return createDefault(GL_TEXTURE_2D);
}

Texture * Texture::createDefault(GLenum target)
{
    Texture* texture = new Texture(target);

    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

void Texture::bind() const
{
    glBindTexture(m_target, id());
}

void Texture::unbind() const
{
    unbind(m_target);
}

void Texture::unbind(const GLenum target)
{
    glBindTexture(target, 0);
}

void Texture::bindActive(const GLenum texture) const
{
    glActiveTexture(texture);
    glBindTexture(m_target, id());
}

void Texture::unbindActive(const GLenum texture) const
{
    glActiveTexture(texture);
    glBindTexture(m_target, 0);
}

GLenum Texture::target() const
{
    return m_target;
}

void Texture::setParameter(GLenum name, GLenum value)
{
    setParameter(name, static_cast<GLint>(value));
}

void Texture::setParameter(GLenum name, GLint value)
{
	bind();

    glTexParameteri(m_target, name, value);
}

void Texture::setParameter(GLenum name, GLfloat value)
{
	bind();

    glTexParameterf(m_target, name, value);
}

GLint Texture::getParameter(GLenum pname) const
{
	bind();

	GLint value = 0;

    glGetTexParameteriv(m_target, pname, &value);

	return value;
}

GLint Texture::getLevelParameter(GLint level, GLenum pname) const
{
	bind();

	GLint value = 0;

    glGetTexLevelParameteriv(m_target, level, pname, &value);

	return value;
}

void Texture::getImage(GLint level, GLenum format, GLenum type, GLvoid * image) const
{
    bind();

    glGetTexImage(m_target, level, format, type, image);
}

std::vector<unsigned char> Texture::getImage(GLint level, GLenum format, GLenum type) const
{
    GLint width = getLevelParameter(level, GL_TEXTURE_WIDTH);
    GLint height = getLevelParameter(level, GL_TEXTURE_HEIGHT);

    int byteSize = imageSizeInBytes(width, height, format, type);

    std::vector<unsigned char> data(byteSize);
    getImage(level, format, type, data.data());

    return data;
}

void Texture::getCompressedImage(GLint lod, GLvoid * image) const
{
    bind();

    glGetCompressedTexImage(m_target, lod, image);
}

std::vector<unsigned char> Texture::getCompressedImage(GLint lod) const
{
    GLint size = getLevelParameter(lod, GL_TEXTURE_COMPRESSED_IMAGE_SIZE);

    std::vector<unsigned char> data(size);
    getCompressedImage(lod, data.data());

    return data;
}

void Texture::image1D(GLint level, GLenum internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * data)
{
    bind();

    glTexImage1D(m_target, level, static_cast<GLint>(internalFormat), width, border, format, type, data);
}

void Texture::compressedImage1D(GLint level, GLenum internalFormat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    bind();

    glCompressedTexImage1D(m_target, level, internalFormat, width, border, imageSize, data);
}

void Texture::subImage1D(GLint level, GLint xOffset, GLsizei width, GLenum format, GLenum type, const GLvoid * data)
{
    bind();

    glTexSubImage1D(m_target, level, xOffset, width, format, type, data);
}

void Texture::image2D(GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
	bind();

    glTexImage2D(m_target, level, static_cast<GLint>(internalFormat), width, height, border, format, type, data);
}

void Texture::image2D(GLint level, GLenum internalFormat, const glm::ivec2 & size, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    image2D(level, internalFormat, size.x, size.y, border, format, type, data);
}

void Texture::image2D(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    bind();

    glTexImage2D(target, level, static_cast<GLint>(internalFormat), width, height, border, format, type, data);
}

void Texture::image2D(GLenum target, GLint level, GLenum internalFormat, const glm::ivec2 & size, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    image2D(target, level, internalFormat, size.x, size.y, border, format, type, data);
}

void Texture::compressedImage2D(GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    bind();

    glCompressedTexImage2D(m_target, level, internalFormat, width, height, border, imageSize, data);
}

void Texture::compressedImage2D(GLint level, GLenum internalFormat, const glm::ivec2 & size, GLint border, GLsizei imageSize, const GLvoid * data)
{
    compressedImage2D(level, internalFormat, size.x, size.y, border, imageSize, data);
}

void Texture::subImage2D(GLint level, GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data)
{
    bind();

    glTexSubImage2D(m_target, level, xOffset, yOffset, width, height, format, type, data);
}

void Texture::subImage2D(GLint level, const glm::ivec2& offset, const glm::ivec2& size, GLenum format, GLenum type, const GLvoid * data)
{
    subImage2D(level, offset.x, offset.y, size.x, size.y, format, type, data);
}

void Texture::image3D(GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    bind();

    glTexImage3D(m_target, level, static_cast<GLint>(internalFormat), width, height, depth, border, format, type, data);
}

void Texture::image3D(GLint level, GLenum internalFormat, const glm::ivec3 & size, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
    image3D(level, internalFormat, size.x, size.y, size.z, border, format, type, data);
}

void Texture::compressedImage3D(GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    bind();

    glCompressedTexImage3D(m_target, level, internalFormat, width, height, depth, border, imageSize, data);
}

void Texture::compressedImage3D(GLint level, GLenum internalFormat, const glm::ivec3 & size, GLint border, GLsizei imageSize, const GLvoid * data)
{
    compressedImage3D(level, internalFormat, size.x, size.y, size.z, border, imageSize, data);
}

void Texture::subImage3D(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * data)
{
    bind();

    glTexSubImage3D(m_target, level, xOffset, yOffset, zOffset, width, height, depth, format, type, data);
}

void Texture::subImage3D(GLint level, const glm::ivec3& offset, const glm::ivec3& size, GLenum format, GLenum type, const GLvoid * data)
{
    subImage3D(level, offset.x, offset.y, offset.z, size.x, size.y, size.z, format, type, data);
}

void Texture::image2DMultisample(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLboolean fixedSamplesLocations)
{
    bind();

    glTexImage2DMultisample(m_target, samples, internalFormat, width, height, fixedSamplesLocations);
}

void Texture::image2DMultisample(GLsizei samples, GLenum internalFormat, const glm::ivec2 & size, GLboolean fixedSamplesLocations)
{
    image2DMultisample(samples, internalFormat, size.x, size.y, fixedSamplesLocations);
}

void Texture::image3DMultisample(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedSamplesLocations)
{
    bind();

    glTexImage3DMultisample(m_target, samples, internalFormat, width, height, depth, fixedSamplesLocations);
}

void Texture::image3DMultisample(GLsizei samples, GLenum internalFormat, const glm::ivec3 & size, GLboolean fixedSamplesLocations)
{
    image3DMultisample(samples, internalFormat, size.x, size.y, size.z, fixedSamplesLocations);
}

void Texture::storage1D(GLsizei levels, GLenum internalFormat, GLsizei width)
{
    bind();

    glTexStorage1D(m_target, levels, internalFormat, width);
}

void Texture::storage2D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	bind();

    glTexStorage2D(m_target, levels, internalFormat, width, height);
}

void Texture::storage2D(GLsizei levels, GLenum internalFormat, const glm::ivec2 & size)
{
    storage2D(levels, internalFormat, size.x, size.y);
}

void Texture::storage3D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
    bind();

    glTexStorage3D(m_target, levels, internalFormat, width, height, depth);
}

void Texture::storage3D(GLsizei levels, GLenum internalFormat, const glm::ivec3 & size)
{
    storage3D(levels, internalFormat, size.x, size.y, size.z);
}

void Texture::textureView(GLuint originalTexture, GLenum internalFormat, GLuint minLevel, GLuint numLevels, GLuint minLayer, GLuint numLayers)
{
    glTextureView(id(), m_target, originalTexture, internalFormat, minLevel, numLevels, minLayer, numLayers);
}

void Texture::texBuffer(GLenum internalFormat, Buffer * buffer)
{
    bind();

    glTexBuffer(m_target, internalFormat, buffer ? buffer->id() : 0);
}

void Texture::texBuffer(GLenum activeTexture, GLenum internalFormat, Buffer * buffer)
{
    bindActive(activeTexture);
    texBuffer(internalFormat, buffer);
}

void Texture::texBufferRange(GLenum internalFormat, Buffer * buffer, GLintptr offset, GLsizeiptr size)
{
    bind();

    glTexBufferRange(m_target, internalFormat, buffer ? buffer->id() : 0, offset, size);
}

void Texture::texBufferRange(GLenum activeTexture, GLenum internalFormat, Buffer * buffer, GLintptr offset, GLsizeiptr size)
{
    bindActive(activeTexture);
    texBufferRange(internalFormat, buffer, offset, size);
}

void Texture::clearImage(GLint level, GLenum format, GLenum type, const void * data)
{
    glClearTexImage(id(), level, format, type, data);
}

void Texture::clearImage(GLint level, GLenum format, GLenum type, const glm::vec4 & value)
{
    clearImage(level, format, type, glm::value_ptr(value));
}

void Texture::clearImage(GLint level, GLenum format, GLenum type, const glm::ivec4 & value)
{
    clearImage(level, format, type, glm::value_ptr(value));
}

void Texture::clearImage(GLint level, GLenum format, GLenum type, const glm::uvec4 & value)
{
    clearImage(level, format, type, glm::value_ptr(value));
}

void Texture::clearSubImage(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * data)
{
    glClearTexSubImage(id(), level, xOffset, yOffset, zOffset, width, height, depth, format, type, data);
}

void Texture::clearSubImage(GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLenum type, const void * data)
{
    clearSubImage(level, offset.x, offset.y, offset.z, size.x, size.y, size.z, format, type, data);
}

void Texture::clearSubImage(GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLenum type, const glm::vec4 & value)
{
    clearSubImage(level, offset, size, format, type, glm::value_ptr(value));
}

void Texture::clearSubImage(GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLenum type, const glm::ivec4 & value)
{
    clearSubImage(level, offset, size, format, type, glm::value_ptr(value));
}

void Texture::clearSubImage(GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLenum type, const glm::uvec4 & value)
{
    clearSubImage(level, offset, size, format, type, glm::value_ptr(value));
}

void Texture::bindImageTexture(GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) const
{
	glBindImageTexture(unit, id(), level, layered, layer, access, format);
}

void Texture::unbindImageTexture(GLuint unit)
{
    // the concrete parameters (except unit & texture) don't seem to matter, as long as their values are valid
    glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
}

void Texture::generateMipmap()
{
	bind();

    glGenerateMipmap(m_target);
}

void Texture::accept(ObjectVisitor& visitor)
{
	visitor.visitTexture(this);
}

TextureHandle Texture::textureHandle() const
{
    return glGetTextureHandleARB(id());
}

bool Texture::isResident() const
{
    return glIsTextureHandleResidentARB(textureHandle()) == GL_TRUE;
}

TextureHandle Texture::makeResident() const
{
    TextureHandle handle = textureHandle();

    glMakeTextureHandleResidentARB(handle);

	return handle;
}

void Texture::makeNonResident() const
{
    glMakeTextureHandleNonResidentARB(textureHandle());
}

void Texture::pageCommitment(GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean commit) const
{
    bind();

    glTexPageCommitmentARB(m_target, level, xOffset, yOffset, zOffset, width, height, depth, commit);
}

void Texture::pageCommitment(GLint level, const glm::ivec3& offset, const glm::ivec3& size, GLboolean commit) const
{
    pageCommitment(level, offset.x, offset.y, offset.z, size.x, size.y, size.z, commit);
}

GLenum Texture::objectType() const
{
    return GL_TEXTURE;
}

} // namespace globjects
