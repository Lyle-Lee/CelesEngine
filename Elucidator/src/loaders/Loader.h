#pragma once

#include <optional>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <glm.hpp>

// Print progress to console while loading (large models)
#define LOADER_CONSOLE_OUTPUT

// Namespace: loader
//
// Description: The namespace that holds eveyrthing that
//	is needed and used for the Model Loader
namespace loader
{
    // A test to see if P1 is on the same side as P2 of a line segment ab
    static bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
    {
        glm::vec3 cp1 = glm::cross(b - a, p1 - a);
        glm::vec3 cp2 = glm::cross(b - a, p2 - a);

        return glm::dot(cp1, cp2) >= 0;
    }

    // Generate a cross produect normal for a triangle
    static glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
    {
        glm::vec3 u = t2 - t1;
        glm::vec3 v = t3 - t1;

        glm::vec3 normal = glm::cross(u, v);

        return glm::normalize(normal);
    }

    // Check to see if a vec3 Point is within a 3 vec3 Triangle
    static bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
    {
        // Test to see if it is within an infinite prism that the triangle outlines.
        bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
            && SameSide(point, tri3, tri1, tri2);

        // If it isn't it will never be on the triangle
        if (!within_tri_prisim)
            return false;

        // Calulate Triangle's Normal
        glm::vec3 n = GenTriNormal(tri1, tri2, tri3);

        return glm::dot(point - tri1, n) == 0;
    }

    // Split a String into a string array at a given token
    inline static void split(const std::string& in, std::vector<std::string>& out, const std::string& token)
    {
        out.clear();

        std::string temp;

        for (int i = 0; i < int(in.size()); i++)
        {
            std::string test = in.substr(i, token.size());

            if (test == token)
            {
                if (!temp.empty())
                {
                    out.push_back(temp);
                    temp.clear();
                    i += (int)token.size() - 1;
                }
                else
                {
                    out.push_back("");
                }
            }
            else if (i + token.size() >= in.size())
            {
                temp += in.substr(i, token.size());
                out.push_back(temp);
                break;
            }
            else
            {
                temp += in[i];
            }
        }
    }

    // Get tail of string after first token and possibly following spaces
    inline static std::string tail(const std::string& in)
    {
        size_t token_start = in.find_first_not_of(" \t");
        size_t space_start = in.find_first_of(" \t", token_start);
        size_t tail_start = in.find_first_not_of(" \t", space_start);
        size_t tail_end = in.find_last_not_of(" \t");
        if (tail_start != std::string::npos && tail_end != std::string::npos)
        {
            return in.substr(tail_start, tail_end - tail_start + 1);
        }
        else if (tail_start != std::string::npos)
        {
            return in.substr(tail_start);
        }
        return "";
    }

    // Get first token of string
    inline static std::string firstToken(const std::string& in)
    {
        if (!in.empty())
        {
            size_t token_start = in.find_first_not_of(" \t");
            size_t token_end = in.find_first_of(" \t", token_start);
            if (token_start != std::string::npos && token_end != std::string::npos)
            {
                return in.substr(token_start, token_end - token_start);
            }
            else if (token_start != std::string::npos)
            {
                return in.substr(token_start);
            }
        }
        return "";
    }

    // Get element at given index position
    template <typename T>
    inline static const T& getElement(const std::vector<T>& elements, const std::string& index)
    {
        int idx = std::stoi(index);
        if (idx < 0)
            idx = int(elements.size()) + idx;
        else
            idx--;
        return elements[idx];
    }
}
