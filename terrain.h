//
//  terrain.h
//  survivalGame
//
//  Created by NewTest on 2021-04-27.
//

#ifndef terrain_h
#define terrain_h

int resolution = 100;
perlin_noise noise;

float noiseLayer(float x, float y, float lacunarity, float persistance, int octaves) {
    
    float freq = .5,
          ampl = 50;
    
    float n = 1;
    
    for (int i = 0; i < octaves; i++) {
        n += noise.noise(x*freq, y*freq, 1230)*ampl;
        freq *= lacunarity;
        ampl *= persistance;
    }
    
    return n;
}

class Terrain {
    
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;
    
    float terrain_vertices[100*100*6];
    unsigned int terrain_indices[99*99*6];
    
    float heights[100][100];
    
public:

    Terrain() {
        
        int indicesIndex = 0;
        
        for (int x = 0; x < resolution; x++) {
            for (int y = 0; y < resolution; y++) {
                int index = y+x*resolution;
                cout << index << endl;
                
                float h = 0;
                h = noiseLayer((float)x/resolution, (float)y/resolution, 2, .5, 16);
                
                //MARK: might be [y][x]
                heights[x][y] = h;
                
                vec3 coord = vec3((x), h, (y));
                vec3 normal = vec3(0, 1, 0);
                
                terrain_vertices[index*6  ] = coord.x;
                terrain_vertices[index*6+1] = coord.y;
                terrain_vertices[index*6+2] = coord.z;
                terrain_vertices[index*6+3] = normal.x;
                terrain_vertices[index*6+4] = normal.y;
                terrain_vertices[index*6+5] = normal.z;
                
                if (x != resolution-1 && y != resolution-1) {
                    terrain_indices[indicesIndex*6  ] = index;
                    terrain_indices[indicesIndex*6+1] = index+1;
                    terrain_indices[indicesIndex*6+2] = index+resolution;
                    terrain_indices[indicesIndex*6+3] = index+1;
                    terrain_indices[indicesIndex*6+4] = index+resolution+1;
                    terrain_indices[indicesIndex*6+5] = index+resolution;
                    indicesIndex++;
                }
            }
        }
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 100*100*6 * sizeof(float), terrain_vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (resolution-1)*(resolution-1)*6 * sizeof(unsigned int), terrain_indices, GL_STATIC_DRAW);
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    }
    
    float baryCentric(vec3 p1, vec3 p2, vec3 p3, vec2 pos) {
        
        float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
        float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
        float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
        
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.y + l2 * p2.y + l3 * p3.y;
    }
    
    float getHeights(float wx, float wz) {
        float terrainX = wx;
        float terrainZ = wz;
        
        float gridSquare = 1; // distance between vertices (on the x and z axis)
        int gridX = (int)floor(terrainX / gridSquare);
        int gridZ = (int)floor(terrainZ / gridSquare);
        
        float xCoord = (fmod(terrainX, gridSquare))/gridSquare;
        float zCoord = (fmod(terrainZ, gridSquare))/gridSquare;
        float result = 0;
        
        if (xCoord <= (1-zCoord)) {
            result = baryCentric(vec3(0, heights[gridX  ][gridZ  ], 0),
                                 vec3(1, heights[gridX+1][gridZ  ], 0),
                                 vec3(0, heights[gridX  ][gridZ+1], 1),
                                 vec2(xCoord, zCoord));
        }
        else {
            result = baryCentric(vec3(1, heights[gridX+1][gridZ  ], 0),
                                 vec3(1, heights[gridX+1][gridZ+1], 1),
                                 vec3(0, heights[gridX  ][gridZ+1], 1),
                                 vec2(xCoord, zCoord));
        }
        return result;
    }

    void render_terrain(Shader &shader) {
        
        shader.use();
        
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(terrain_vertices), terrain_vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrain_indices), terrain_indices, GL_STATIC_DRAW);
                        
        glDrawElements(RenderingType, (resolution-1)*(resolution-1)*6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
};
#endif /* terrain_h */
