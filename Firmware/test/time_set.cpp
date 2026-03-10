#include <ThreeWire.h>  
#include <RtcDS1302.h>

// --- DS1302 RTC Pins ---
#define RTC_DAT   27  // I/O pin
#define RTC_CLK   26  // SCLK pin
#define RTC_RST   14  // CE / RST pin

ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("\n--- RTC Time Setter ---");

  // 1. Start communication
  Rtc.Begin();

  // 2. Force the chip to unlock and start ticking
  Rtc.SetIsWriteProtected(false);
  Rtc.SetIsRunning(true);

  // 3. MANUALLY SET THE TIME HERE
  // Format: Year, Month, Day, Hour, Minute, Second
  RtcDateTime targetTime = RtcDateTime(2026, 3, 10, 22, 11, 0);
  
  // 4. Send the command to write the time to the chip
  Rtc.SetDateTime(targetTime); 
  
  Serial.println("Time successfully injected into DS1302!");
}

void loop() {
  // Read the time every second and print it so we can verify it is ticking
  RtcDateTime now = Rtc.GetDateTime();
  
  char timeString[25];
  snprintf(timeString, sizeof(timeString), "%04d/%02d/%02d %02d:%02d:%02d", 
           now.Year(), now.Month(), now.Day(), 
           now.Hour(), now.Minute(), now.Second());

  Serial.print("Current RTC Time: ");
  Serial.println(timeString);
  
  delay(1000);
}