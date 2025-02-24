#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "input/input_manager.h"

namespace DF::Input
{
    class GLFWInputManager : public IInputManager
    {
    private:
        std::map<int, Key> m_glfwKeyMap{
            { GLFW_KEY_ESCAPE, Key::ESC },
            { GLFW_KEY_ENTER,  Key::ENTER },
            { GLFW_KEY_SPACE,  Key::SPACE },
        };
        std::map<Key, std::vector<std::function<void()>>> m_keyCallbacksMap{};

    public:
        //GLFWInputManager() = delete;

        //explicit GLFWInputManager(const Render::Window& window) noexcept
        //{
        //    glfwSetKeyCallback(window.getWindow(), glfwKeyCallback);
        //}

        bool keyPressed(Key key) const override;

        bool mouseButtonPressed(Key key) const override;

        void onKeyPress(Key key, std::function<void()>) override;

        void onMouseButtonPress(MouseButton button, std::function<void()>) override;

        void update() override;

    private:
        Key getMappedKeyGLFW(int key) const;

        static void glfwKeyCallback(GLFWwindow*, int key, int, int, int);
    };

    bool GLFWInputManager::keyPressed(Key key) const
    {
        return false;
    }

    bool GLFWInputManager::mouseButtonPressed(Key key) const
    {
        return false;
    }

    void GLFWInputManager::onKeyPress(Key key, std::function<void()> callback)
    {
        m_keyCallbacksMap[key].push_back(std::move(callback));
    }

    void GLFWInputManager::onMouseButtonPress(MouseButton button, std::function<void()> callback)
    {

    }

    void GLFWInputManager::update()
    {

    }

    Key GLFWInputManager::getMappedKeyGLFW(int key) const
    {
        auto it{ m_glfwKeyMap.find(key) };

        if (it == m_glfwKeyMap.end()) return Key::UNDEFINED;

        return it->second;
    }

    void GLFWInputManager::glfwKeyCallback(GLFWwindow*, int key, int, int, int)
    {

    }

    static std::shared_ptr<GLFWInputManager> inputManager{};

    std::shared_ptr<IInputManager> getInputManager()
    {
        if (!inputManager) {
            inputManager = std::make_shared<GLFWInputManager>();
        }

        return inputManager;
    }
}
