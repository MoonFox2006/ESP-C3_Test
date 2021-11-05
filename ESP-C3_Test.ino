#include <Arduino.h>
#include <driver/temp_sensor.h>

void setup() {
  Serial.begin(115200);
  Serial.println();

  for (uint8_t pin = 0; pin < 6; ++pin)
    pinMode(pin, INPUT);

  if (esp_reset_reason() == ESP_RST_DEEPSLEEP) {
    Serial.print("Waked up by ");
    switch (esp_sleep_get_wakeup_cause()) {
      case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("TIMER");
        break;
      case ESP_SLEEP_WAKEUP_GPIO:
        Serial.println("GPIO");
        {
          bool pressed;

          do {
            for (uint8_t pin = 0; pin < 6; ++pin) {
              if (pressed = digitalRead(pin) == (pin != 2))
                break;
            }
            if (pressed)
              delay(1);
          } while (pressed);
        }
        break;
      default:
        Serial.print("UNKNOWN (");
        Serial.print(esp_sleep_get_wakeup_cause());
        Serial.println(')');
        break;
    }
  }

  {
    temp_sensor_config_t cfg = TSENS_CONFIG_DEFAULT();

    temp_sensor_set_config(cfg);
    temp_sensor_start();
  }
  delay(1000); // 1 sec.
  {
    float t;

    if (temp_sensor_read_celsius(&t) == ESP_OK)
      Serial.printf("CPU core temperature: %.4f C\r\n", t);
  }

  esp_deep_sleep_disable_rom_logging();
  esp_sleep_enable_timer_wakeup(10000000); // 10 sec.
  esp_deep_sleep_enable_gpio_wakeup(BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(5), ESP_GPIO_WAKEUP_GPIO_HIGH);
  esp_deep_sleep_enable_gpio_wakeup(BIT(2), ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.println("Going to deep sleep...");
  Serial.flush();

  esp_deep_sleep_start();
}

void loop() {}
