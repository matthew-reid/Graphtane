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

#include <GAppFramework/Application.h>
#include <GAppFramework/RenderQueueIds.h>
#include <GComputeVis/Convert.h>

#include <GSparseVolumes/RenderableVolumeFactory.h>
#include <GSparseVolumesVdb/GridNormalCalculator.h>
#include <GSparseVolumesVdb/VdbUtil.h>
#include <GSparseVolumesVdb/VdbGrid.h>

#include <GCommon/Logger.h>
#include <GVis/Camera.h>
#include <GVis/VisSystem.h>
#include <GVis/Window.h>
#include <GVis/Primitives/BoxMeshFactory.h>
#include <GVis/Io/MaterialFactory.h>
#include <GVis/Io/ObjLoader.h>
#include <GVis/Material.h>
#include <GVis/Mesh.h>
#include <GVis/Geo.h>
#include <GVis/ShaderProgram.h>
#include <GVis/Light.h>
#include <GVis/ShaderParameter.h>
#include <GVis/RenderableNode.h>
#include <GVis/RenderTextureTarget.h>
#include <GVis/Technique.h>
#include <GVis/Texture.h>
#include <GVis/Viewport.h>
#include <GVis/Io/DdsLoader.h>
#include <GVis/Primitives/RectangleMeshFactory.h>

#include <openvdb/openvdb.h>
#include <openvdb/tools/Interpolation.h>

#include <exception>

#include <boost/foreach.hpp>


using namespace GAppFramework;
using namespace GCommon;
using namespace GSparseVolumes;
using namespace GVis;

enum GridTextureRole
{
	GridTextureRole_Diffuse,
	GridTextureRole_Normal,
	GridTextureRole_Temperature
};

typedef std::map<GridTextureRole, GridPtr> GridTextureRolesMap;

class SparseVolumeMaterialFactoryI : public SparseVolumeMaterialFactory
{
public:
	SparseVolumeMaterialFactoryI(const GridTextureRolesMap& gridRoles, bool transparent, float opacityMultiplier) :
		m_gridRoles(gridRoles),
		m_transparent(transparent),
		m_opacityMultiplier(opacityMultiplier)
	{
		ShaderProgramConfig config("Shaders/Volume/VdbRaymarchedVolume.vert", "Shaders/Volume/VdbRaymarchedVolume.frag");
		
		// Set shader macro definitions
		if (m_gridRoles.find(GridTextureRole_Normal) != m_gridRoles.end())
		{
			config.macroDefinitions.push_back("USE_NORMAL_SAMPLER");
		}

		if (m_gridRoles.find(GridTextureRole_Temperature) != m_gridRoles.end())
		{
			config.macroDefinitions.push_back("USE_TEMPERATURE_SAMPLER");
		}

		// Create shader
		m_volumeShader = ShaderProgram::createShaderProgram(config);
		m_temperatureRampTexture = DdsLoader::load("Textures/TemperatureRamp.dds", ColorSpace_SRGB);
		m_temperatureRampTexture->setTextureAddressMode(TextureAddressMode_Clamp);
	}


	//! Implementation of VolumeRenderableMaterialFactory
	MaterialPtr createMaterial(const SparseVolumeMaterialConfig& config) const
	{
		TexturePtr densityTexture = getGridForRole(config.leafAtlases, GridTextureRole_Diffuse);
		assert(densityTexture);

		TexturePtr normalTexture = getGridForRole(config.leafAtlases, GridTextureRole_Normal);
		TexturePtr temperatureTexture = getGridForRole(config.leafAtlases, GridTextureRole_Temperature);

		MaterialPtr material(new Material);

		glm::vec3 densityTextureSize(densityTexture->getWidth(), densityTexture->getHeight(), densityTexture->getDepth());

		// Main technique
		{
			TechniquePtr technique(new Technique(m_volumeShader));
			{
				Vec3ShaderParameterPtr boxModelSizeParameter(new Vec3ShaderParameter("volumeSize_modelSpace", config.boxSize));
				technique->addCustomShaderParameter(boxModelSizeParameter);

				Vec3ShaderParameterPtr oneOnVolumeTextureSizeParameter(new Vec3ShaderParameter("oneOnVolumeTextureSize", 1.0f / densityTextureSize));
				technique->addCustomShaderParameter(oneOnVolumeTextureSizeParameter);

				technique->setAlphaBlendingMode(AlphaBlendingMode_PreMultiplied);

				int maxLeafCountPerInternalNode = config.maxLeafCountPerInternalNodeDimension * config.maxLeafCountPerInternalNodeDimension * config.maxLeafCountPerInternalNodeDimension;

				technique->addCustomShaderParameter(ShaderParameterPtr(new IntShaderParameter("firstInternalNodeIndex", config.firstInternalNodeIndex)));
				technique->addCustomShaderParameter(ShaderParameterPtr(new IntShaderParameter("maxLeafCountPerInternalNode", maxLeafCountPerInternalNode)));
				technique->addCustomShaderParameter(ShaderParameterPtr(new IVec3ShaderParameter("maxLeafCountPerAtlasDimension", config.maxLeafCountPerAtlasDimension)));
				technique->addCustomShaderParameter(ShaderParameterPtr(new IntShaderParameter("maxLeafCountPerInternalNodeDimension", config.maxLeafCountPerInternalNodeDimension)));
				
				float thresholdAlpha = m_transparent ? 0.001 : 0.2;
				technique->addCustomShaderParameter(ShaderParameterPtr(new FloatShaderParameter("thresholdAlpha", thresholdAlpha)));

				float opacityMultiplier = m_transparent ? m_opacityMultiplier : 200.0;
				technique->addCustomShaderParameter(ShaderParameterPtr(new FloatShaderParameter("opacityMultiplier", opacityMultiplier)));

				{
					TextureUnit unit(densityTexture, "albedoSampler");
					technique->addTextureUnit(unit);
				}

				if (normalTexture)
				{
					TextureUnit unit(normalTexture, "normalSampler");
					technique->addTextureUnit(unit);
				}

				if (temperatureTexture)
				{
					TextureUnit unit(temperatureTexture, "temperatureSampler");
					technique->addTextureUnit(unit);
				}

				TextureUnit unit(m_temperatureRampTexture, "temperatureRampSampler");
				technique->addTextureUnit(unit);

				{
					TextureUnit unit(config.nodeIndirectionTexture, "nodeIndirectionSampler");
					technique->addTextureUnit(unit);
				}
			}

			material->setTechnique(technique);
		}

		return material;
	}

private:
	TexturePtr getGridForRole(const GridTexturesMap& textures, GridTextureRole role) const
	{
		TexturePtr result;

		GridTextureRolesMap::const_iterator i = m_gridRoles.find(role);
		if (i != m_gridRoles.end())
		{
			GridTexturesMap::const_iterator j = textures.find(i->second);
			if (j != textures.end())
			{
				return j->second;
			}
		}

		return result;
	}

private:
	ShaderProgramPtr m_volumeShader;
	TexturePtr m_temperatureRampTexture;
	GridTextureRolesMap m_gridRoles;
	bool m_transparent;
	float m_opacityMultiplier;
};

struct DemoAppConfig : ApplicationConfig
{
	DemoAppConfig() :
		generateNormals(false),
		transparent(false),
		orbitCam(true),
		renderToLowResTarget(true),
		opacityMultiplier(1.0)
	{
	}

	std::string vdbVilename;
	std::string densityGridName;
	std::string temperatureGridName; //! will not be used if empty

	float opacityMultiplier;

	bool generateNormals;
	bool transparent;
	
	//! When enabled, orbits camera around rendered volume
	bool orbitCam;

	//! When enabled, improves performance by rendering output to a low res full-screen texture
	bool renderToLowResTarget;
};

class DemoApplication : public Application
{
public:
	DemoApplication(const DemoAppConfig& config) :
		Application(config),
		m_config(config)
	{
	}

	RenderTextureTargetPtr createDepthTarget(const CameraPtr& camera, int width, int height)
	{
		ImageTextureConfig config = ImageTextureConfig::createDefault();
		config.width = width;
		config.height = height;
		config.textureAddressMode = TextureAddressMode_Clamp;
		config.format = PixelFormat_DepthComponent24;

		TexturePtr texture(new Texture(config));

		RenderTextureTargetPtr target;

		{
			RenderTextureTargetConfig config;
			config.texture = texture;
			config.attachment = FrameBufferAttachment_Depth;
			config.techniqueCategory = TechniqueCategory_DepthRtt;
			target.reset(new RenderTextureTarget(config));

			{
				ViewportPtr viewport = target->addDefaultViewport(m_camera);
				viewport->setBackgroundColor(glm::vec4(1,1,1,1));
			}

			m_window->addRenderTarget(target);
		}
		return target;
	}

	struct CopyBIntoA {
		inline void operator()(const float& a, const float& b, float& result) const {
			result = a;
		}
	};

	void setupScene()
	{
		m_window->getViewports().front()->setRenderQueueIdMask(~RenderQueueId_OffscreenTransparentObjects);

		LightPtr light(new Light);
		light->setDirection(glm::normalize(glm::vec3(1.5, -1, -0.5)));
		light->setPosition(glm::vec3(0, 0.5, 0) + light->getDirection() * -10.0f);
		m_visSystem->addLight(light);

		// Volume
		std::vector<std::string> gridNames;
		gridNames.push_back(m_config.densityGridName);

		if (!m_config.temperatureGridName.empty())
		{
			gridNames.push_back(m_config.temperatureGridName);
		}

		std::vector<openvdb::GridBase::Ptr> baseGrids = loadGridsFromFile(m_config.vdbVilename, gridNames);

		openvdb::FloatGrid::Ptr densityGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrids[0]);

		openvdb::FloatGrid::Ptr temperatureGrid;
		if (!m_config.temperatureGridName.empty())
		{
			temperatureGrid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrids[1]);

			// Rescale temperature field to between 0 and 1
			{
				float minValue = 99999999999999;
				float maxValue = -99999999999999;
				for (openvdb::FloatGrid::ValueOnCIter iter = temperatureGrid->cbeginValueOn(); iter.test(); ++iter) {
					float value = *iter;
					minValue = std::min(minValue, value);
					maxValue = std::max(maxValue, value);
				}

				float scale = 1.0 / (maxValue - minValue);

				for (openvdb::FloatGrid::ValueOnIter iter = temperatureGrid->beginValueOn(); iter.test(); ++iter) {
					float value = *iter;
					value = (value - minValue) * scale;
					iter.setValue(value);
				}
			}

			// Ensure both grids have the same hierarchy
			{
				openvdb::FloatGrid::Ptr resultGrid = openvdb::FloatGrid::create();
				resultGrid->tree().combine2(densityGrid->tree(), temperatureGrid->tree(), CopyBIntoA());
				temperatureGrid->tree().combine(densityGrid->tree(), CopyBIntoA());

				densityGrid = resultGrid; 
			}
		}

		std::vector<GridPtr> grids;
		GridTextureRolesMap gridTextureRoles;

		grids.push_back(GridPtr(new VdbGrid<openvdb::FloatGrid>(densityGrid, 1)));
		gridTextureRoles[GridTextureRole_Diffuse] = grids.back();

		if (m_config.generateNormals)
		{
			Vec3UByteGrid::Ptr normalGrid = createNormalGrid(*densityGrid);
			grids.push_back(GridPtr(new VdbGrid<Vec3UByteGrid>(normalGrid, 3)));
			gridTextureRoles[GridTextureRole_Normal] = grids.back();
		}

		if (temperatureGrid)
		{
			grids.push_back(GridPtr(new VdbGrid<openvdb::FloatGrid>(temperatureGrid, 1)));
			gridTextureRoles[GridTextureRole_Temperature] = grids.back();
		}

		openvdb::CoordBBox bbox;
		densityGrid->constTree().evalLeafBoundingBox(bbox);

		float scale = 1.0f / (float)bbox.extents().asVec3s().length();

		glm::vec3 center = toVec3(bbox.getCenter() * scale);
		std::ostringstream ss;
		ss << "Grid extents: " << bbox.extents().x() << ", " << bbox.extents().y() << ", " << bbox.extents().z();
		defaultLogger()->logLine(ss.str());

		int renderQueueId;
		if (m_config.renderToLowResTarget)
		{
			renderQueueId = RenderQueueId_OffscreenTransparentObjects;
		}
		else
		{
			renderQueueId = getDefaultRenderQueueId();
		}

		RenderableVolumeConfig config;
		config.grids = grids;
		config.materialFactory.reset(new SparseVolumeMaterialFactoryI(gridTextureRoles, m_config.transparent, m_config.opacityMultiplier));
		config.scale = scale;
		config.batchBoxes = !m_config.transparent; // don't batch if we have transparency so we can sort

		RenderableVolumePtr volume = RenderableVolumeFactory::createRenderableVolume(config);
		BOOST_FOREACH(const GVis::RenderableNodePtr& node, volume->nodes)
		{
			node->translate(-center);
			m_visSystem->addRenderableNode(node, renderQueueId);
		}

		if (m_config.transparent)
		{
			RenderQueuePtr renderQueue = m_visSystem->getRenderQueue(renderQueueId);
			if (renderQueue)
				renderQueue->setSortingMode(RenderSortingMode_BackToFront);
		}

		if (m_config.renderToLowResTarget)
		{
			int volumeTargetWidth = m_window->getWidth() / 2;
			int volumeTargetHeight = m_window->getHeight() / 2;

			TexturePtr offscreenTransparentObjectsTexture;
			// Create offscreen transparent objects target
			{
				ImageTextureConfig config = ImageTextureConfig::createDefault();
				config.width = volumeTargetWidth;
				config.height = volumeTargetHeight;
				config.textureAddressMode = TextureAddressMode_Clamp;
				offscreenTransparentObjectsTexture.reset(new Texture(config));

				{
					RenderTextureTargetConfig config;
					config.texture = offscreenTransparentObjectsTexture;
					config.attachment = FrameBufferAttachment_Color;
					RenderTextureTargetPtr target(new RenderTextureTarget(config));

					{
						ViewportPtr viewport = target->addDefaultViewport(m_camera);
						viewport->setRenderQueueIdMask(RenderQueueId_OffscreenTransparentObjects);
						viewport->setBackgroundColor(glm::vec4(0,0,0,0));
					}

					m_window->addRenderTarget(target);
				}
			}

			// Composite offscreen particles
			{
				TextureUnit textureUnit(offscreenTransparentObjectsTexture, "albedoSampler");

				ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Common/ScreenQuad.vert", "Shaders/Common/SimpleTextured.frag"));
				TechniquePtr technique(new Technique(shader));
				technique->addTextureUnit(textureUnit);
				technique->setAlphaBlendingMode(AlphaBlendingMode_PreMultiplied);
				MaterialPtr material(new Material);
				material->setTechnique(technique);

				{
					RectangleConfig config = RectangleConfig::defaultWithSize(glm::vec2(2, 2));
					MeshPtr mesh = RectangleMeshFactory::createMesh(config);
					GeoPtr geo(new Geo(mesh, material));
					
					RenderableNodePtr renderableNode = RenderableNode::createWithSingleGeo(geo);
					m_visSystem->addRenderableNode(renderableNode, RenderQueueId_CompositeOffscreenTransparentObjects);
				}
			}
		}

		if (m_config.orbitCam)
		{
			centerNode.reset(new SceneNode);
			centerNode->addChild(m_camera);
			m_cameraInputEnabled = false;
		}
		m_camera->setPosition_parentSpace(glm::vec3(0,0,1.4));
	}

	void render()
	{

		if (m_config.orbitCam)
		{
			float azimuth = m_elapsedTime;
			float elevation = 0.0;

			glm::quat ori = glm::angleAxis(azimuth, glm::vec3(0, 1, 0)) * glm::angleAxis(elevation, glm::vec3(1, 0, 0));
			centerNode->setOrientation(ori);
		}

		Application::render();
	}

	void simulate(float dt) {}

private:
	SceneNodePtr centerNode;
	DemoAppConfig m_config;
	bool m_orbitCam;
};

#include <boost/program_options.hpp>

using namespace boost;

int main(int argc, char** argv)
{
	try
	{
		namespace po = boost::program_options;

		DemoAppConfig config;

		// read command line options
		po::options_description description("VdbViewer");
		description.add_options()
		("help,h", "displays help")
		("vdbFile,v", po::value<std::string>(&config.vdbVilename)->required(), "vdb filename")
		("densityGridName,d", po::value<std::string>(&config.densityGridName)->default_value("density"), "density grid name")
		("temperatureGridName", po::value<std::string>(&config.temperatureGridName)->default_value("temperature"), "temperature grid name")
		("useTemperatureGrid", "volume will be shaded using temperature grid")
		("opacityMultiplier,o", po::value<float>(&config.opacityMultiplier), "opacity multiplier to use with transparent rendering")
		("generateNormals,n", "generate normals")
		("orbit,c", "orbit camera")
		("lowres,r", "render to low resolution framebuffer")
		("transparent,t", "render as transparent volume");

		po::variables_map vm;
		po::store(program_options::command_line_parser(argc, argv).options(description).run(), vm);

		if ( vm.count("help")  ) 
		{ 
			std::cout << description << std::endl; 
		}
		else
		{
			po::notify(vm);

			config.generateNormals = vm.count("generateNormals");
			config.transparent = vm.count("transparent");
			config.orbitCam = vm.count("orbit");
			config.renderToLowResTarget = vm.count("lowres");
			
			if (!vm.count("useTemperatureGrid"))
			{
				config.temperatureGridName.clear();
			}
			config.windowConfig.antiAliasingSampleCount = 0; // MSAA is probably overkill, disable it for performance

			DemoApplication app(config);
			app.run();
		}
	}
	catch(const std::exception& e)
	{
		defaultLogger()->logLine(e.what());
	}
}