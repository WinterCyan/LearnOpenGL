//
//  learnglm.cpp
//  OpenGL
//
//  Created by Winter Cyan on 2019/7/6.
//  Copyright © 2019 Winter Cyan. All rights reserved.
//

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void learnglm(){
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans2 = glm::mat4(1.0);
    trans2 = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans2*vec;
    std::cout<<vec.x<<" "<<vec.y<<" "<<vec.z<<" "<<std::endl;
}
