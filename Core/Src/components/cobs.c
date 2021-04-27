/**
  ******************************************************************************
  * @file		cobs.c
  * @brief		(De)codificación en COBS.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cobs.h"

/**
  * @brief  Codifica un paquete de bytes en COBS con 0x00 como term char
  *         (no incluído).
  * @param  decodedMessage	Puntero al buffer con el paquete de bytes a
  * 						codificar.
  * @param  lenght	Especifica el número de bytes a codificar.
  * @param	codedMessage	Puntero al buffer donde se almacenará el paquete de
  * 						bytes codificado.
  * @return Número de bytes resultantes de la codificación.
  */
uint32_t COBS_encode(uint8_t *decodedMessage, uint32_t lenght, uint8_t *codedMessage) {

  uint32_t read_index = 0,
		   write_index = 1,
		   code_index = 0;

  uint8_t code = 0x01;

  while(read_index < lenght) {

    if (decodedMessage[read_index] == 0x00) {

      codedMessage[code_index] = code;
      code_index = write_index;
      write_index++;
      code = 0x01;

    } else {

      codedMessage[write_index] = decodedMessage[read_index];
      write_index++;
      code++;

      if (code == 0xFF) {

        codedMessage[code_index] = code;
        code_index = write_index;
        code = 0x01;
        if (read_index + 1 < lenght) write_index++;
      }
    }

    read_index++;

  }

  codedMessage[code_index] = code;

  return write_index;

}

/**
  * @brief  Decodifica un paquete de bytes en COBS con 0x00 como term char
  *         (no incluído).
  * @param  codedMessage	Puntero al buffer con el paquete de bytes a
  * 						decodificar.
  * @param  lenght	Especifica el número de bytes a decodificar.
  * @param	decodedMessage	Puntero al buffer donde se almacenará el paquete de
  * 						bytes decodificado.
  * @return Número de bytes resultantes de la decodificación.
  */
uint32_t COBS_decode(uint8_t *codedMessage, uint32_t lenght, uint8_t *decodedMessage) {

  uint32_t read_index = 0,
           write_index = 0;

  while(read_index < lenght) {

    uint8_t code = codedMessage[read_index];
    read_index++;

    for (int i = 1; i < code; i++) {
      decodedMessage[write_index] = codedMessage[read_index];
      write_index++;
      read_index++;
    }

    if (code < 0xFF && read_index < lenght) {
      decodedMessage[write_index] = 0x00;
      write_index++;
    }
  }

  return write_index;

}
