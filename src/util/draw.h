#pragma once

#include "common.h"
#include <rlgl.h>

// Most of these functions are just copied from here but with a zPos parameter added
// https://github.com/raysan5/raylib/blob/e5d332dea23e65f66e7e7b279dc712afeb9404c9/src/rshapes.c

// Draw a line  (Vector version)
inline void DrawLineV3D(Vector2 startPos, Vector2 endPos, Color color, float zPos)
{
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex3f(startPos.x, startPos.y, zPos);
    rlVertex3f(endPos.x, endPos.y, zPos);
    rlEnd();
}

// Draw lines sequence
inline void DrawLineStrip3D(Vector2 *points, int pointCount, Color color, float zPos)
{
    if (pointCount >= 2)
    {
        rlBegin(RL_LINES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 0; i < pointCount - 1; i++)
        {
            rlVertex3f(points[i].x, points[i].y, zPos);
            rlVertex3f(points[i + 1].x, points[i + 1].y, zPos);
        }
        rlEnd();
    }
}

// Draw a color-filled rectangle with pro parameters
inline void DrawRectanglePro3D(Rectangle rec, Vector2 origin, float rotation, Color color, float zPos)
{
    Vector2 topLeft = { 0 };
    Vector2 topRight = { 0 };
    Vector2 bottomLeft = { 0 };
    Vector2 bottomRight = { 0 };

    // Only calculate rotation if needed
    if (rotation == 0.0f)
    {
        float x = rec.x - origin.x;
        float y = rec.y - origin.y;
        topLeft = (Vector2){ x, y };
        topRight = (Vector2){ x + rec.width, y };
        bottomLeft = (Vector2){ x, y + rec.height };
        bottomRight = (Vector2){ x + rec.width, y + rec.height };
    }
    else
    {
        float sinRotation = sinf(rotation*DEG2RAD);
        float cosRotation = cosf(rotation*DEG2RAD);
        float x = rec.x;
        float y = rec.y;
        float dx = -origin.x;
        float dy = -origin.y;

        topLeft.x = x + dx*cosRotation - dy*sinRotation;
        topLeft.y = y + dx*sinRotation + dy*cosRotation;

        topRight.x = x + (dx + rec.width)*cosRotation - dy*sinRotation;
        topRight.y = y + (dx + rec.width)*sinRotation + dy*cosRotation;

        bottomLeft.x = x + dx*cosRotation - (dy + rec.height)*sinRotation;
        bottomLeft.y = y + dx*sinRotation + (dy + rec.height)*cosRotation;

        bottomRight.x = x + (dx + rec.width)*cosRotation - (dy + rec.height)*sinRotation;
        bottomRight.y = y + (dx + rec.width)*sinRotation + (dy + rec.height)*cosRotation;
    }

    rlBegin(RL_TRIANGLES);

    rlColor4ub(color.r, color.g, color.b, color.a);

    rlVertex3f(topLeft.x, topLeft.y, zPos);
    rlVertex3f(bottomLeft.x, bottomLeft.y, zPos);
    rlVertex3f(topRight.x, topRight.y, zPos);

    rlVertex3f(topRight.x, topRight.y, zPos);
    rlVertex3f(bottomLeft.x, bottomLeft.y, zPos);
    rlVertex3f(bottomRight.x, bottomRight.y, zPos);

    rlEnd();
}

// Draw a color-filled rectangle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues
inline void DrawRectangleV3D(Vector2 position, Vector2 size, Color color, float zPos)
{
    DrawRectanglePro3D((Rectangle){ position.x, position.y, size.x, size.y }, (Vector2){ 0.0f, 0.0f }, 0.0f, color, zPos);
}

// Draw a triangle fan defined by points
// NOTE: First vertex provided is the center, shared by all triangles
// By default, following vertex should be provided in counter-clockwise order
inline void DrawTriangleFan3D(Vector2 *points, int pointCount, Color color, float zPos)
{
    if (pointCount >= 3)
    {
        rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 1; i < pointCount - 1; i++)
        {
            rlVertex3f(points[0].x, points[0].y, zPos);
            rlVertex3f(points[i].x, points[i].y, zPos);
            rlVertex3f(points[i + 1].x, points[i + 1].y, zPos);
            rlVertex3f(points[i + 1].x, points[i + 1].y, zPos);
        }
        rlEnd();
    }
}

// Draws a texture in 3D space with pro parameters...
inline void DrawTexturePro3D(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector3 origin, float rotation, float posZ, Color tint)
{
    // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        bool flipX = false;

        if (sourceRec.width < 0) { flipX = true; sourceRec.width *= -1; }
        if (sourceRec.height < 0) sourceRec.y -= sourceRec.height;

        rlSetTexture(texture.id);
        rlPushMatrix();
        rlTranslatef(destRec.x, destRec.y, 0.0f);
        rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
        rlTranslatef(-origin.x, -origin.y, -origin.z);

        rlBegin(RL_QUADS);
        rlColor4ub(tint.r, tint.g, tint.b, tint.a);
        rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

        // Bottom-left corner for texture and quad
        if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
        else rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
        rlVertex3f(0.0f, 0.0f, posZ);

        // Bottom-right corner for texture and quad
        if (flipX) rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
        else rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
        rlVertex3f(0.0f, destRec.height, posZ);

        // Top-right corner for texture and quad
        if (flipX) rlTexCoord2f(sourceRec.x/width, (sourceRec.y + sourceRec.height)/height);
        else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, (sourceRec.y + sourceRec.height)/height);
        rlVertex3f(destRec.width, destRec.height, posZ);

        // Top-left corner for texture and quad
        if (flipX) rlTexCoord2f(sourceRec.x/width, sourceRec.y/height);
        else rlTexCoord2f((sourceRec.x + sourceRec.width)/width, sourceRec.y/height);
        rlVertex3f(destRec.width, 0.0f, posZ);
        rlEnd();
        rlPopMatrix();
        rlSetTexture(0);
    }
}