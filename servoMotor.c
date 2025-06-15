
#include <16F877A.h>
#fuses XT, NOWDT, NOPROTECT, BROWNOUT, PUT, NOLVP
#use delay(clock=4000000) // 4 MHz kristal



#define PIR1 PIN_B0 // Ýlk hareket sensörü (Giriþ)
#define PIR2 PIN_B1 // Ýkinci hareket sensörü (Giriþ)
#define IR1 PIN_B2 // Ýlk kýzýlötesi sensör (Giriþ)
#define IR2 PIN_B3 // Ýkinci kýzýlötesi sensör (Giriþ)
#define SERVO PIN_C2 // Servo motor (PWM çýkýþý)
#define LED1 PIN_C0 //LED1 çýkýþ
#define LED2 PIN_C1 //LED2 çýkýþ
#define LED3 PIN_C3 //LED3 çýkýþ
#define LED4 PIN_C4 //LED4 çýkýþ
#define LED5 PIN_C5 //LED5 çýkýþ
#define LED6 PIN_C6 //LED6 çýkýþ

#include <lcd.c> // Dahili LCD kütüphanesi

volatile int pir1_flag = 0; // PIR1 durumu için bayrak
volatile int pir2_flag = 0; // PIR2 durumu için bayrak
volatile unsigned long time_elapsed = 0; // Zaman ölçümü için deðiþken kaç defa cevrim yaptýðýný anlamak için
volatile int ir1_state = 0; // IR1 için önceki durum
volatile int ir2_state = 0; // IR2 için önceki durum 
unsigned int arac_sayisi = 0; // Geçen araç sayýsý

void led_kayar_ileri() {
    output_high(LED1);
    delay_ms(100);
    output_high(LED2);
    delay_ms(100);
    output_high(LED3);
    delay_ms(100);
    output_high(LED4);
    delay_ms(100);
    output_high(LED5);
    delay_ms(100);
    output_high(LED6);
    delay_ms(10); //iþlemciyi meþgul etmemesi için delay süresini minimimda tutuk
    
}

void led_kayar_geri() {
    output_low(LED1);
    delay_ms(100);
    output_low(LED2);
    delay_ms(100);
    output_low(LED3);
    delay_ms(100);
    output_low(LED4);
    delay_ms(100);
    output_low(LED5);
    delay_ms(100);
    output_low(LED6);
    delay_ms(10); //iþlemciyi meþgul etmemesi için delay süresini minimimda tutuk
    
}

void servo_ac() {
    set_pwm1_duty(90);
}

void servo_kapat() {
    set_pwm1_duty(0);
}

void main() {
    // Port yönlerini ayarla
    set_tris_b(0x0F); // PORTB: RB0-RB3 giriþ, diðerleri çýkýþ
    set_tris_c(0x00); // PORTC çýkýþ PWM ve LED ler için
    output_c(0x00); // baþlangýç durumu olarak lojik 0 a ayarlýyorum C çýkýþlaýný

    // PWM ve Timer ayarlarý
    setup_timer_0(RTCC_DIV_256); // 256 prescaler
    setup_timer_2(T2_DIV_BY_16, 249, 1); // PWM frekansýný ayarla (50 Hz)
    setup_ccp1(CCP_PWM); // PWM modunu etkinleþtir

    // LCD ayarlarý
    lcd_init(); // LCD baþlat
    lcd_putc("\fMERHABA \nHOSGELDINIZ... "); // LCD ekraný temizle ve baþlangýç mesajýný yazdýr

    while (1) {
        // IR1 sensörü için düþen kenar tespiti
        if (ir1_state == 1 && input(IR1) == 0) { // IR1 düþen kenar algýlandý
            servo_ac(); // Servo motoru açýlýr
        }
        ir1_state = input(IR1); // IR1'in mevcut durumunu kaydet

        // IR2 sensörü için düþen kenar tespiti
        if (ir2_state == 1 && input(IR2) == 0) { // IR2 düþen kenar algýlandý
            servo_kapat(); // Servo motoru kapatýlýr
            arac_sayisi++; // Araç sayýsýný artýr

            // LCD'yi güncelle
            lcd_putc("\fGecen ");
            printf(lcd_putc, "%u. aracsizin", arac_sayisi); // Güncel araç sayýsýný yazdýr
        }
        ir2_state = input(IR2); // IR2'nin mevcut durumunu kaydet
        
        // PIR1 sensörü tetiklenmiþse
        if (input(PIR1) && !pir1_flag) {
            pir1_flag = 1; // Bayraðý ayarla
            set_timer0(0); // Timer0 sýfýrla
            led_kayar_ileri(); // LED'leri sýrayla yak
        }

        // PIR2 sensörü tetiklenmiþse
        if (input(PIR2) && !pir2_flag) {
            pir2_flag = 1; // Bayraðý ayarla
            time_elapsed = get_timer0(); // Geçen zamaný al
            //led_kayar_geri();

            // Hýz hesapla (hýz = mesafe / zaman)
             // = 1 metre, zaman birimi mikrosaniye (prescaler ile ayarlý)
            float time_in_seconds = (float)time_elapsed * 256.0 / 4000000.0; // Zamaný saniyeye çevir
            float speed =1.0 / time_in_seconds; // Hýz (Km/h deðil m/s ye çevirdim)
           

            // LCD'yi güncelle
            lcd_putc("\fGULE GULE \nHiziniz "); // LCD'yi temizle ve baþlýðý yaz
            printf(lcd_putc, "%.2fm/s", speed); // Güncel hýzý yazdýr
            led_kayar_geri();
        }



        // PIR sensör bayraklarýný sýfýrla
        if (!input(PIR1)) pir1_flag = 0;
        if (!input(PIR2)) pir2_flag = 0;
        
    }
}
