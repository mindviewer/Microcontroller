#include "display.h"
#include "GUI.h"

/**
 * @brief       DUI LCD display Initialize
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to initialize GUI LCD display.
 */
void Display_Init(void)
{
		GUI_Init();
}

/**
 * @brief       Display the buffer information on LCD
 *
 * @param[in]   *buf        Character Array Buffer. ex. char buf[n]
 * @param[in]   x  					x_axis.range (0-)
 * @param[in]   y  					y_axis.range (0-)
 *
 * @return      None
 *
 * @details     This function is used to display the buffer information on LCD.
 */
void Display_buf(char *buf, int x, int y)
{
		GUI_SetFont(&GUI_Font16_ASCII);
		GUI_DispStringAt(buf, x, y);
}
