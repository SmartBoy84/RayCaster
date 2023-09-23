#include "pixel.h"
#include "font.h"

uint32_t getOverlayColor(struct Window *window, int index, Color newColor)
{
    uint32_t color = window->pixels[index];

    if (newColor[3] == 255)
        return COLOR_TO_UINT32(newColor);

    else if (newColor[3] == 0)
        return COLOR_TO_UINT32(newColor);

    float scalar = (float)newColor[3] / 255.0;

    uint8_t red = (color & 0xFF000000) >> 24;
    uint8_t green = (color & 0x00FF0000) >> 16;
    uint8_t blue = (color & 0x0000FF00) >> 8;

    return ((uint32_t)((uint8_t)(red + (newColor[0] - red) * scalar) << 16 | // start from the old colour then use the alpha value as the percentage for where in the range between the two colours to place the new colur
                       (uint8_t)(green + (newColor[1] - green) * scalar) << 8 |
                       (uint8_t)(blue + (newColor[2] - blue) * scalar)));
}

void SetScreen(struct Window *window, Color color)
{
    for (int i = 0; i < GET_INDEX(window, window->size.width - 1, window->size.height - 1); i++)
        window->pixels[i] = COLOR_TO_UINT32(color);
}

void ChangePixel(struct Window *window, int posX, int posY, Color color)
{
    if (posX >= window->size.width || posX < 0 || posY >= window->size.height || posY < 0)
        return;

    int index = GET_INDEX(window, posX, posY);
    window->pixels[index] = getOverlayColor(window, index, color);
}

void DrawRect(struct Window *window, int posX, int posY, int width, int height, Color fillColor)
{
    if (!IS_TRANSPARENT(fillColor))
    {
        for (int y = posY; y < posY + height; y++)
            for (int x = posX; x < posX + width; x++)
                ChangePixel(window, x, y, fillColor);
    }
}

int DrawLetter(struct Window *window, char character, int posX, int posY, int scalarPx, Color textColor, Color backColor)
{
    if (character > sizeof(font))
    {
        LogMessage("Character not supported!");
        return 1;
    }

    for (int y = 0; y < FONT_HEIGHT; y++)
        for (int x = 0; x < FONT_WIDTH; x++)
            DrawRect(window, posX + x * scalarPx, posY + y * scalarPx, scalarPx, scalarPx, (font[character][y] >> x) & 1 ? textColor : backColor);
    return 0;
}

void DrawString(struct Window *window, char *string, int posX, int posY, int scalarPx, int characterWarp, Color textColor, Color backColor)
{
    if (!scalarPx)
        return;

    int charLen = 0;

    int y = posY;
    int x = posX;

    int width = scalarPx * FONT_WIDTH;
    int height = scalarPx * FONT_HEIGHT;

    for (int i = 0; i < strlen(string); i++)
    {
        if (DrawLetter(window, string[i], x, y, scalarPx, textColor, backColor))
            printf("Warning, character %c not specified in font", string[i]);

        charLen++;
        if (characterWarp && charLen == characterWarp)
        {
            y += height;
            charLen = 0;
        }
        else
            x += width;
    }
}