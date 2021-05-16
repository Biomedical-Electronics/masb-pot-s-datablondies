#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"


//================================== I2C =======================================
// Inicializamos la libreria I2C. Ya no hay que hacer nada mas con ella.

// Inicializamos la libreria. La variable hi2c1 nos la genera STM32Cube cuando 
// configuremos el I2C en el archivo .ioc.
I2C_Init(&hi2c1);

// En el archivo 'Inc/components/i2c_lib.h' hay un #define I2C_DEBUG puesto para
// que no se transmita nada por I2C mientras no tengamos la placa. De este modo
// podemos hacer pruebas. Cuando se tenga la placa, hay que eliminar o comentar
// esta linea.


//=========================== Potenciometro ====================================
// Solo debemos de ejecutar este codigo una unica vez al inicio del programa.
// Fijaremos una resistencia fija para siempre y no la modificaremos.

// Creamos el handle de la libreria.
AD5280_Handle_T hpot = NULL;

hpot = AD5280_Init();

// Configuramos su direccion I2C de esclavo, su resistencia total (hay
// diferentes modelos; este tiene 50kohms) e indicamos que funcion queremos que
// se encargue de la escritura a traves del I2C. Utilizaremos la funcion
// I2C_Write de la libreria i2c_lib.
AD5280_ConfigSlaveAddress(hpot, 0x2C);
AD5280_ConfigNominalResistorValue(hpot, 50e3f);
AD5280_ConfigWriteFunction(hpot, I2C_Write);

// Fijamos la resistencia de, por ejemplo, 12kohms.
AD5280_SetWBResistance(hpot, 12e3f));


//================================== DAC =======================================
// Esto lo realizaremos una unica vez al inicio del programa. ------------------

// Creamos el handle de la libreria.
MCP4725_Handle_T hdac = NULL;

hdac = MCP4725_Init();

// Configuramos la direccion I2C de esclavo, su tension de referencia (que es la
// de alimentacion) e indicamos que funcion queremos que se encargue de la
// escritura a traves del I2C. Utilizaremos la funcion I2C_Write de la libreria
// i2c_lib.
MCP4725_ConfigSlaveAddress(hdac, 0x66);
MCP4725_ConfigVoltageReference(hdac, 4.0f);
MCP4725_ConfigWriteFunction(hdac, I2C_Write);

// Esto lo ejecutamos cada vez que queremos generar una tension nueva. ---------
MCP4725_SetOutputVoltage(hdac, 0.0f);
