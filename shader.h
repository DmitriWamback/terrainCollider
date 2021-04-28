//
//  shader.h
//  survivalGame
//
//  Created by NewTest on 2021-04-27.
//

#ifndef shader_h
#define shader_h

class Shader {
public:
    
    int program;
    
    Shader(const char* vertexShaderPath = nullptr, const char* fragmentShaderPath = nullptr) {
            
        if (vertexShaderPath && fragmentShaderPath) {
            
            ifstream _vertexShader, _fragmentShader;
            stringstream vShaderStream, fShaderStream;
            
            _vertexShader.open(vertexShaderPath);
            _fragmentShader.open(fragmentShaderPath);
            vShaderStream << _vertexShader.rdbuf();
            fShaderStream << _fragmentShader.rdbuf();
            
            _vertexShader.close();
            _fragmentShader.close();
            
            int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            
            string a = vShaderStream.str(), b = fShaderStream.str();
            
            const char* vsrc = a.c_str();
            const char* fsrc = b.c_str();
            
            cout << vsrc;
                                    
            glShaderSource(vertexShader, 1, &vsrc, NULL);
            glShaderSource(fragmentShader, 1, &fsrc, NULL);
            glCompileShader(vertexShader);
            glCompileShader(fragmentShader);
            
            program = glCreateProgram();
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);
            cout << program << endl;
            
            int success;
            char infoLog[1024];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    
    void set_vec3(int location, vec3 &vector) {
            glUniform3fv(location, 1, &vector[0]);
        }
        
        void set_mat4(int location, mat4 &matrix) {
            glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
        }
        
        void set_int(int location, int a) {
            glUniform1f(location, a);
        }
        
        void use() {
            glUseProgram(program);
        }
};

#endif /* shader_h */
