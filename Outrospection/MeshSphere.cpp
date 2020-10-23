#include "MeshSphere.h"

#include <map>

#define X .525731112119133606
#define Z .850650808352039932

static glm::vec3 vdata[12] = {
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

static GLint tindices[20][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
//--------------------------------------------------------------------------------

MeshSphere::MeshSphere(unsigned int levels)
{
    // init with an icosahedron
    for (auto& i : vdata)
        mVertices.push_back({i});

    mIndices.push_back(new std::vector<int>);
    std::vector<int>& indices = *mIndices.back();
    for (auto& indexTri : tindices)
    {
        for (int& k : indexTri)
            indices.push_back(k);
    }
    mListIds.push_back(0);

    while (mIndices.size() < levels)
        subdivide();

    int counter = 0;

    for(int index : *mIndices[levels - 1])
    {
        Vertex& v = mVertices[index];

        int vThing = counter % 3;

        switch (vThing) {
        case 0:
                v.texCoord = glm::vec2(0.5, 0.0);
                break;
        case 1:
                v.texCoord = glm::vec2(0.0, 1.0);
                break;
        case 2:
                v.texCoord = glm::vec2(1.0, 1.0);
        }

        counter++;
    }

    name = "sphere";
    indicesSize = static_cast<unsigned int>((mIndices[levels - 1])->size());

    Resource res = Resource("Textures/", "testSS00.png");
    texture = Outrospection::get().textureManager.get(res);

    // create buffers and arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLuint), &(mIndices[levels - 1]->at(0)), GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // norms
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    // tex coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
    // tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
    // bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

    glBindVertexArray(0);
}

const std::vector<int>& MeshSphere::indices(int level) const
{
    while (level >= int(mIndices.size()))
        const_cast<MeshSphere*>(this)->subdivide();
    return *mIndices[level];
}

void MeshSphere::subdivide(void)
{
    PROFILE;

    typedef unsigned long long Key;
    std::map<Key, int> edgeMap;
    const std::vector<int>& indices = *mIndices.back();
    mIndices.push_back(new std::vector<int>);
    std::vector<int>& refinedIndices = *mIndices.back();
    int end = indices.size();
    for (int i = 0; i < end; i += 3)
    {
        int ids0[3],  // indices of outer vertices
            ids1[3];  // indices of edge vertices
        for (int k = 0; k < 3; ++k)
        {
            int k1 = (k + 1) % 3;
            int e0 = indices[i + k];
            int e1 = indices[i + k1];
            ids0[k] = e0;
            if (e1 > e0)
                std::swap(e0, e1);
            Key edgeKey = Key(e0) | (Key(e1) << 32);
            auto it = edgeMap.find(edgeKey);
            if (it == edgeMap.end())
            {
                ids1[k] = mVertices.size();
                edgeMap[edgeKey] = ids1[k];
                mVertices.push_back(Vertex { glm::normalize(mVertices[e0].pos + mVertices[e1].pos) });
            }
            else
                ids1[k] = it->second;
        }
        refinedIndices.push_back(ids0[0]); refinedIndices.push_back(ids1[0]); refinedIndices.push_back(ids1[2]);
        refinedIndices.push_back(ids0[1]); refinedIndices.push_back(ids1[1]); refinedIndices.push_back(ids1[0]);
        refinedIndices.push_back(ids0[2]); refinedIndices.push_back(ids1[2]); refinedIndices.push_back(ids1[1]);
        refinedIndices.push_back(ids1[0]); refinedIndices.push_back(ids1[1]); refinedIndices.push_back(ids1[2]);
    }
    mListIds.push_back(0);
}