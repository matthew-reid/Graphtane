// Copyright (c) 2013-2014 Matthew Paul Reid

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GVis {

using std::tr1::static_pointer_cast;
using std::tr1::shared_ptr;

class Camera;
class Compositor;
class DynamicTexture;
class FloatShaderParameter;
class Geo;
class Light;
class Mat4ShaderParameter;
class Material;
class MaterialFactory;
class Mesh;
struct MeshData;
class OrthographicProjection;
class PerspectiveProjection;
class Projection;
class Renderable;
class RenderableNode;
struct RenderContext;
class RenderQueue;
class RenderTarget;
class RenderTargetListener;
class RenderTextureTarget;
class SceneNode;
class ScopedTextureBufferObject;
class ShaderParameter;
class ShaderProgram;
class ShadowProjector;
class Technique;
class Texture;
class TextureBufferObject;
class Vec2ShaderParameter;
class Vec3ShaderParameter;
class Viewport;
class VisSystem;
class Window;

typedef shared_ptr<Camera> CameraPtr;
typedef shared_ptr<Compositor> CompositorPtr;
typedef shared_ptr<DynamicTexture> DynamicTexturePtr;
typedef shared_ptr<FloatShaderParameter> FloatShaderParameterPtr;
typedef shared_ptr<Geo> GeoPtr;
typedef shared_ptr<Light> LightPtr;
typedef shared_ptr<Mat4ShaderParameter> Mat4ShaderParameterPtr;
typedef shared_ptr<Material> MaterialPtr;
typedef shared_ptr<Mesh> MeshPtr;
typedef shared_ptr<OrthographicProjection> OrthographicProjectionPtr;
typedef shared_ptr<PerspectiveProjection> PerspectiveProjectionPtr;
typedef shared_ptr<Projection> ProjectionPtr;
typedef shared_ptr<Renderable> RenderablePtr;
typedef shared_ptr<RenderableNode> RenderableNodePtr;
typedef shared_ptr<RenderQueue> RenderQueuePtr;
typedef shared_ptr<RenderTarget> RenderTargetPtr;
typedef shared_ptr<RenderTargetListener> RenderTargetListenerPtr;
typedef shared_ptr<RenderTextureTarget> RenderTextureTargetPtr;
typedef shared_ptr<SceneNode> SceneNodePtr;
typedef shared_ptr<ScopedTextureBufferObject> ScopedTextureBufferObjectPtr;
typedef shared_ptr<ShaderParameter> ShaderParameterPtr;
typedef shared_ptr<ShaderProgram> ShaderProgramPtr;
typedef shared_ptr<ShadowProjector> ShadowProjectorPtr;
typedef shared_ptr<Technique> TechniquePtr;
typedef shared_ptr<Texture> TexturePtr;
typedef shared_ptr<TextureBufferObject> TextureBufferObjectPtr;
typedef shared_ptr<Viewport> ViewportPtr;
typedef shared_ptr<VisSystem> VisSystemPtr;
typedef shared_ptr<Vec2ShaderParameter> Vec2ShaderParameterPtr;
typedef shared_ptr<Vec3ShaderParameter> Vec3ShaderParameterPtr;
typedef shared_ptr<Window> WindowPtr;

typedef int RenderQueueIdMask;
typedef int RenderQueueId;

} // namespace GVis
