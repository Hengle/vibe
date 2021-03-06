/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#pragma once

#include <unordered_map>
#include <memory>

#include "imstkSceneObject.h"
#include "imstkLight.h"
#include "imstkIBLProbe.h"
#include "imstkCamera.h"
#include "imstkCollisionGraph.h"
#include "imstkSolverBase.h"

#include "imstkcpdScene.h"

namespace imstk
{
class SceneObjectControllerBase;
class DebugRenderGeometry;

enum class TimeSteppingPolicy
{
    asFastAsPossible,
    fixedFrameRate,
    realTime
};

struct SceneConfig
{
    // Initializes the scene only when it needs to frame
    // Note: May cause delays to run the first frame of the scene due to scene initialization
    bool lazyInitialization = false;

    TimeSteppingPolicy timeStepping = TimeSteppingPolicy::asFastAsPossible;

    // Keep track of the fps for the scene
    bool trackFPS = false;
};

///
/// \class Scene
///
/// \brief
///
class Scene
{
template<class T>
using NamedMap = std::unordered_map<std::string, std::shared_ptr<T>>;

public:

    ///
    /// \brief Constructor
    ///
    Scene(const std::string& name, std::shared_ptr<SceneConfig> config = std::make_shared<SceneConfig>()) : m_name(name), m_config(config) {}
    Scene(std::string&& name, std::shared_ptr<SceneConfig> config = std::make_shared<SceneConfig>()) : m_name(std::move(name)), m_config(config) {}

    ///
    /// \brief Destructor
    ///
    ~Scene();

    ///
    /// \brief Initialize the scene
    ///
    bool initialize();

    ///
    /// \brief Launch camera controller and other scene specific modules that need to run independently
    ///
    void launchModules();

    ///
    /// \brief Reset the scene
    ///
    void reset();
    void resetSceneObjects();

	const cpd::ScenePtr getCPDScene() { return m_CPDScene; }
	void setCPDScene(cpd::ScenePtr p_CPDScene) { m_CPDScene = p_CPDScene; }

    ///
    /// \brief Advance the scene from current to next frame
    ///
    void advance();

    ///
    /// \brief Advance the scene from current to next frame with specified timestep
    ///
    void advance(const double dt);

    ///
    /// \brief Returns true if the object with a given name is registered, else false
    ///
    bool isObjectRegistered(const std::string& sceneObjectName) const;

    ///
    /// \brief Return a vector of shared pointers to the scene objects
    /// NOTE: A separate list might be efficient as this is called runtime
    ///
    const std::vector<std::shared_ptr<SceneObject>> getSceneObjects() const;

    ///
    /// \brief Return a vector of shared pointers to the scene objects
    /// NOTE: A separate list might be efficient as this is called runtime
    ///
    const std::vector<std::shared_ptr<DebugRenderGeometry>> getDebugRenderObjects() const;

    ///
    /// \brief Get the scene object controllers
    ///
    const std::vector<std::shared_ptr<SceneObjectControllerBase>> getSceneObjectControllers() const;

    ///
    /// \brief Get a scene object of a specific name
    ///
    std::shared_ptr<SceneObject> getSceneObject(const std::string& sceneObjectName) const;

    ///
    /// \brief Add/remove a scene object
    ///
    void addSceneObject(std::shared_ptr<SceneObject> newSceneObject);
    void addDebugGeometry(std::shared_ptr<DebugRenderGeometry> newSceneObject);
    void removeSceneObject(const std::string& sceneObjectName);

    ///
    /// \brief
    ///
    bool isLightRegistered(const std::string& lightName) const;

    ///
    /// \brief Return a vector of lights in the scene
    ///
    const std::vector<std::shared_ptr<Light>> getLights() const;

    ///
    /// \brief Get a light with a given name
    ///
    std::shared_ptr<Light> getLight(const std::string& lightName) const;

    ///
    /// \brief Add/remove lights from the scene
    ///
    void addLight(std::shared_ptr<Light> newLight);
    void removeLight(const std::string& lightName);

    ///
    /// \brief Add/remove lights from the scene
    ///
    void setGlobalIBLProbe(std::shared_ptr<IBLProbe> newIBLProbe);
    std::shared_ptr<IBLProbe> getGlobalIBLProbe();

    ///
    /// \brief Get the name of the scene
    ///
    const std::string& getName() const;

    ///
    /// \brief Get the camera for the scene
    ///
    std::shared_ptr<Camera> getCamera() const;

    ///
    /// \brief Return the collision graph
    ///
    std::shared_ptr<CollisionGraph> getCollisionGraph() const;

    ///
    /// \brief Get the vector of non-linear solvers
    ///
    const std::vector<std::shared_ptr<SolverBase>> getSolvers();

    ///
    /// \brief Add nonlinear solver to the scene
    ///
    void addNonlinearSolver(std::shared_ptr<SolverBase> solver);

    ///
    /// \brief Add objects controllers
    ///
    void addObjectController(std::shared_ptr<SceneObjectControllerBase> controller);

    ///
    /// \brief
    ///
    bool isInitialized() const { return m_isInitialized; }

    ///
    /// \brief Set/Get the FPS
    ///
    void setFPS(const double fps) { m_fps = fps; }
    double getFPS() { return m_fps; }

    ///
    /// \brief Get the configuration
    ///
    std::shared_ptr<const SceneConfig> getConfig() const { return m_config; };
    const std::shared_ptr<SceneConfig> getConfig() { return m_config; };

protected:
    std::shared_ptr<SceneConfig> m_config;

    std::string                              m_name; ///> Name of the scene
    NamedMap<SceneObject>                    m_sceneObjectsMap;
    NamedMap<DebugRenderGeometry>            m_DebugRenderGeometryMap;
    NamedMap<Light>                          m_lightsMap;
    std::shared_ptr<IBLProbe>                m_globalIBLProbe = nullptr;
    std::shared_ptr<Camera>                  m_camera         = std::make_shared<Camera>();
    std::shared_ptr<CollisionGraph>          m_collisionGraph = std::make_shared<CollisionGraph>();
    std::vector<std::shared_ptr<SolverBase>> m_solvers;                          ///> List of non-linear solvers
    std::vector<std::shared_ptr<SceneObjectControllerBase>> m_objectControllers; ///> List of controllers
    std::unordered_map<std::string, std::thread> m_threadMap;                    ///>

	cpd::ScenePtr m_CPDScene;

    double m_fps       = 0.0;
    double elapsedTime = 0.0;

    bool m_isInitialized = false;

    std::atomic<bool> m_resetRequested = ATOMIC_VAR_INIT(false);
};
} // imstk
