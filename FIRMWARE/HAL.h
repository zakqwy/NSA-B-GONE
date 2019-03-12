#ifndef HAL_H_
#define HAL_H_

#define PORT_IO         PORTC
#define PIN_IO          PINC
#define CAM_MIC         PC0
#define LED_HDD_IN      PC1
#define LED_HDD_OUT     PC2
#define THINKLIGHT_IN   PC3

void SystemInit(void);
void CamMicOff(void);
void CamMicOn(void);
void LedOn(void);
void LedOff(void);

#endif /* HAL_H_ */
