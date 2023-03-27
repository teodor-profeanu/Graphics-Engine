#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN};
    
    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, float sensitivity);
        Camera();
        glm::mat4 getViewMatrix();
        glm::mat4 getViewMatrixFromOrigin();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float xpos, float ypos);
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        
    private:
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        glm::vec3 direction;
        float lastX=0, lastY=0, sensitivity, yaw=0, pitch=0;
    };
    
}

#endif
