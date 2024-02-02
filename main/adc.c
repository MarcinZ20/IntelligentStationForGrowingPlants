#include "include/adc.h"

/*Description:
 * Function to configure the ADC
 *
 * @Input: [void]
 *
 * @Output: [void] Configures ADC
 **/
static void config_adc(void) {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL, ADC_ATTEN_DB_0);
}
