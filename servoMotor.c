
#include <16F877A.h>
#fuses XT, NOWDT, NOPROTECT, BROWNOUT, PUT, NOLVP
#use delay(clock=4000000) // 4 MHz kristal



#define PIR1 PIN_B0 // �lk hareket sens�r� (Giri�)
#define PIR2 PIN_B1 // �kinci hareket sens�r� (Giri�)
#define IR1 PIN_B2 // �lk k�z�l�tesi sens�r (Giri�)
#define IR2 PIN_B3 // �kinci k�z�l�tesi sens�r (Giri�)
#define SERVO PIN_C2 // Servo motor (PWM ��k���)
#define LED1 PIN_C0 //LED1 ��k��
#define LED2 PIN_C1 //LED2 ��k��
#define LED3 PIN_C3 //LED3 ��k��
#define LED4 PIN_C4 //LED4 ��k��
#define LED5 PIN_C5 //LED5 ��k��
#define LED6 PIN_C6 //LED6 ��k��

#include <lcd.c> // Dahili LCD k�t�phanesi

volatile int pir1_flag = 0; // PIR1 durumu i�in bayrak
volatile int pir2_flag = 0; // PIR2 durumu i�in bayrak
volatile unsigned long time_elapsed = 0; // Zaman �l��m� i�in de�i�ken ka� defa cevrim yapt���n� anlamak i�in
volatile int ir1_state = 0; // IR1 i�in �nceki durum
volatile int ir2_state = 0; // IR2 i�in �nceki durum 
unsigned int arac_sayisi = 0; // Ge�en ara� say�s�

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
    delay_ms(10); //i�lemciyi me�gul etmemesi i�in delay s�resini minimimda tutuk
    
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
    delay_ms(10); //i�lemciyi me�gul etmemesi i�in delay s�resini minimimda tutuk
    
}

void servo_ac() {
    set_pwm1_duty(90);
}

void servo_kapat() {
    set_pwm1_duty(0);
}

void main() {
    // Port y�nlerini ayarla
    set_tris_b(0x0F); // PORTB: RB0-RB3 giri�, di�erleri ��k��
    set_tris_c(0x00); // PORTC ��k�� PWM ve LED ler i�in
    output_c(0x00); // ba�lang�� durumu olarak lojik 0 a ayarl�yorum C ��k��la�n�

    // PWM ve Timer ayarlar�
    setup_timer_0(RTCC_DIV_256); // 256 prescaler
    setup_timer_2(T2_DIV_BY_16, 249, 1); // PWM frekans�n� ayarla (50 Hz)
    setup_ccp1(CCP_PWM); // PWM modunu etkinle�tir

    // LCD ayarlar�
    lcd_init(); // LCD ba�lat
    lcd_putc("\fMERHABA \nHOSGELDINIZ... "); // LCD ekran� temizle ve ba�lang�� mesaj�n� yazd�r

    while (1) {
        // IR1 sens�r� i�in d��en kenar tespiti
        if (ir1_state == 1 && input(IR1) == 0) { // IR1 d��en kenar alg�land�
            servo_ac(); // Servo motoru a��l�r
        }
        ir1_state = input(IR1); // IR1'in mevcut durumunu kaydet

        // IR2 sens�r� i�in d��en kenar tespiti
        if (ir2_state == 1 && input(IR2) == 0) { // IR2 d��en kenar alg�land�
            servo_kapat(); // Servo motoru kapat�l�r
            arac_sayisi++; // Ara� say�s�n� art�r

            // LCD'yi g�ncelle
            lcd_putc("\fGecen ");
            printf(lcd_putc, "%u. aracsizin", arac_sayisi); // G�ncel ara� say�s�n� yazd�r
        }
        ir2_state = input(IR2); // IR2'nin mevcut durumunu kaydet
        
        // PIR1 sens�r� tetiklenmi�se
        if (input(PIR1) && !pir1_flag) {
            pir1_flag = 1; // Bayra�� ayarla
            set_timer0(0); // Timer0 s�f�rla
            led_kayar_ileri(); // LED'leri s�rayla yak
        }

        // PIR2 sens�r� tetiklenmi�se
        if (input(PIR2) && !pir2_flag) {
            pir2_flag = 1; // Bayra�� ayarla
            time_elapsed = get_timer0(); // Ge�en zaman� al
            //led_kayar_geri();

            // H�z hesapla (h�z = mesafe / zaman)
             // = 1 metre, zaman birimi mikrosaniye (prescaler ile ayarl�)
            float time_in_seconds = (float)time_elapsed * 256.0 / 4000000.0; // Zaman� saniyeye �evir
            float speed =1.0 / time_in_seconds; // H�z (Km/h de�il m/s ye �evirdim)
           

            // LCD'yi g�ncelle
            lcd_putc("\fGULE GULE \nHiziniz "); // LCD'yi temizle ve ba�l��� yaz
            printf(lcd_putc, "%.2fm/s", speed); // G�ncel h�z� yazd�r
            led_kayar_geri();
        }



        // PIR sens�r bayraklar�n� s�f�rla
        if (!input(PIR1)) pir1_flag = 0;
        if (!input(PIR2)) pir2_flag = 0;
        
    }
}
