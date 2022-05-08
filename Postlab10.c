/*
 * File:   Postlab10.c
 * Author: Josea
 *
 * Created on 4 de mayo de 2022, 06:05 PM
 */



// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000
#define LEN_MSG 28               // Constante para definir largo de mensaje e iteraciones al enviarlo por el serial
#define IN_MIN 0                // Valor minimo de entrada del potenciometro
#define IN_MAX 255              // Valor máximo de entrada del potenciometro
#define OUT_MIN 0               // Valor minimo de ancho de pulso de señal PWM
#define OUT_MAX 500             // Valor máximo de ancho de pulso de señal PWM

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
char mensaje = 0;
uint8_t indice = 0;             // Variable para saber que posición del mensaje enviar al serial
char valor_old = 0;          // Variable para guardar el valor anterior recibido
uint8_t pot = 0;          // Variable para guardar el valor anterior recibido




/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);
void cadena(char palabra[]);
void leerpot(void);
void enviarASCII(void);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    if(PIR1bits.ADIF){              // Fue interrupción del ADC?
        if(ADCON0bits.CHS == 0){    // Verificamos sea AN0 el canal seleccionado
            pot = ADRESH;         // Mostramos los bits superiores en PORTD
            
        }
        
        PIR1bits.ADIF = 0;          // Limpiamos bandera de interrupción
    }
    
    if(PIR1bits.RCIF){          // Hay datos recibidos?
        mensaje = RCREG;     // Guardamos valor recibido en el arreglo mensaje
        TXREG = mensaje;
        if(mensaje = 49){
            leerpot();
        }
        if(mensaje = 50){
            enviarASCII();
        }
    }
    
    
        
    return;
}

/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){
        //ADC
        if(ADCON0bits.GO == 0){             // No hay proceso de conversion
           ADCON0bits.GO = 1;              // Iniciamos proceso de conversión
        }
       
    }  
    
    
}


/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){

    ANSEL = 0b00000001;         // AN0
    ANSELH = 0;                 // I/O digitales
    
    TRISA = 0b00000001;         //AN0 como entrada
    PORTA = 0;
    
    TRISB = 0;
    PORTB = 0;                  // PORTB como salida
    
    TRISD = 0;
    PORTD = 0;                  // PORTB como salida
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    // Configuraciones de comunicacion serial
    //SYNC = 0, BRGH = 1, BRG16 = 1, SPBRG=25 <- Valores de tabla 12-5
    TXSTAbits.SYNC = 0;         // Comunicación ascincrona (full-duplex)
    TXSTAbits.BRGH = 1;         // Baud rate de alta velocidad 
    BAUDCTLbits.BRG16 = 1;      // 16-bits para generar el baud rate
    
    SPBRG = 25;
    SPBRGH = 0;                 // Baud rate ~9600, error -> 0.16%
    
    RCSTAbits.SPEN = 1;         // Habilitamos comunicación
    TXSTAbits.TX9 = 0;          // Utilizamos solo 8 bits
    TXSTAbits.TXEN = 1;         // Habilitamos transmisor
    RCSTAbits.CREN = 1;         // Habilitamos receptor
    
    // Configuraciones de interrupciones
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitamos Interrupciones de recepción
    PIE1bits.ADIE = 1;          // Habilitamos int. de ADC
    PIR1bits.ADIF = 0;          // Limpiamos bandera de int. ADC
    
    // Configuración ADC
    ADCON0bits.ADCS = 0b01;     // Fosc/8
    
    ADCON1bits.VCFG0 = 0;       // VDD *Referencias internas
    ADCON1bits.VCFG1 = 1;       // VSS
    
    ADCON0bits.CHS = 0b0000;    // Seleccionamos AN0
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON0bits.ADON = 1;        // Habilitamos modulo ADC
    __delay_us(40);
}




void cadena(char palabra[]){
    uint8_t i=0;
        while(palabra[i]!=' '){             
            TXREG = palabra[i];    
            i++;
            //__delay_ms(300);
            } 
        return;
    
}

void leerpot(void){
    while(RCREG == 49){
        PORTD = pot;
        TXREG = PORTD;
        //cadena("POT");
        __delay_ms(1500);
    }
   
   
}

void enviarASCII (void){
    while(RCREG == 50){
        PORTB = RCREG;
        //cadena("ASCII");
    }
    
}