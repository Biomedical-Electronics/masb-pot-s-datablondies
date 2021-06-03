# Proyecto: Potenciostato

En el presente documento se detalla la programación de un potenciostato portable para poder realizar dos tipos de mediciones electroquímicas: voltammetría cíclica y cronoamperometría. 

## Contenidos

- [Introducción](#introducción)
- [Objetivos](#objetivos)
- [Resultados](#resultados)
    - [Voltammetría](#voltammetria)
    - [Amperometría](#amperometria)   
- [Conclusiones](#conclusiones)

## Introducción

Con los avances en miniaturización es posible diseñar complejos SoC (Systems-on-Chip) en espacios reducidos, y obtener dispositivos portátiles con un gran impacto en el _healthcare_. Para ello, los biopotenciostatos son de las partes más importantes que componen un biosensor. Este controla una celda compuesta de tres electrodos: el electrodo de trabajo (WE), el de referencia (RE), y un electrodo auxiliar (AUX). El circuito manteniene el potencial del electrodo de trabajo a un nivel constante con respecto al potencial del electrodo de referencia, mediante el ajuste de la corriente en el electrodo auxiliar. 

Este componente es fundamental para aquellos estudios electroquímicos con sistemas de tres electrodos empleados en el estudio de reacciones redox y otros fenómenos químicos. 

Una de las medidas electroquímicas es la Voltammetria Cíclica (CV), un tipo de medición potenciodinámica, es decir, dónde se aplica un potencial variable a una celda electroquímica. Por otro lado, se mide la corriente que esta celda proporciona y se representa frente al voltaje aplicado. El potencial se mide entre el electrodo de trabajo y el electrodo de referencia, mientras que la corriente se mide entre el electrodo de trabajo y el auxiliar. El potencial del electrodo aumenta linealmente en función del tiempo en las fases cíclicas hasta que alcanza un valor y cambia de dirección. Este mecanismo se denomina barrido triangular de potencial y se puede ver representado en la siguiente figura x. La tasa de cambio de voltaje a lo largo del tiempo durante cada una de estas fases se conoce como velocidad de exploración (V/s).

La otra medida tratada en este proyecto es la Cronoamperometría (CA). En esta se aplica un señal escalón, elevando el valor del potencial a una tal que ocurre una reacción redox. Y entonces, se mide la variación de la respuesta de la corriente en función del tiempo. 

Explicar....

- Pinout: pines del microcontrolador usados para controlar el front-end
- Front-end:
    - PMU
    - Relé
    - Potenciostato
        - DAC
        - ADC
        - TIA
- Aplicación
    - viSens-S
    - Microcontrolador
        - Perifericos
        - Configuración de medida
        - Timers
        - Obtención de datos
        - Envio de datos  


## Objetivos

- Programar un potenciostato portable.
- Controlar la Power Management Unit (PMU) del módulo front-end del potenciostato.
- Comunicarse con la aplicación viSens-S instalada con el host u ordenador mediante el protocolo MASB-COMM-S.
- Realizar una voltammetría cíclica.
- Realizar una cronoamperometría.
- Implementar la branching policy para el control de versiones. 

## Resultados
Intro resultados.

### Voltammetría

### Amperometría

## Conclusiones
