
# DDR Demo on PolarFire SoC

This example project demonstrates the use of the MPFS HAL for DDR bring-up.
The application uses a serial terminal for user interaction. It prints
informative messages on the serial port at the start-up providing details of
the DDR initialization. Once DDR is trained, from the E51 application code a
menu to select further test options is presented.

## Target boards:
This example project is targeted at a number of different boards
 - Icicle kit
 - DDR base board with DDR3 installed
 - DDR base board with DDR4 installed
 - IOF Max verification board ( LPDDR3 )
 - Peripheral Base board

The boards other than the Icicle kit are internal Microchip boards. The DDR settings  
for these boards are a guide on how to configure your board if based on one
of those DDR technologies.

| Board                     | DDR memory Type| Data width | Speed       |
| :-------------            | :----------  | :----------  |:----------  |
|  Icicle                   | LPDDR4       |  x32         |  1600MHz    |
|  Icicle 666MHz            | LPDDR4       |  x32         |  1333MHz    |
|  Peripheral Base board    | LPDDR4       |  x16         |  1600MHz    |
|  Peripheral Base board    | LPDDR4       |  x32         |  1600MHz    |
|  DDR Base Board           | DDR4         |  x32         |  1600MHz    |
|  DDR Base Board           | DDR3         |  x32         |  1333MHz    |

Note: DDR3 low voltage ( DDR3L ) is also supported. This is the same from an
operational/ algorithmic point of view. The only difference are the voltage
setting set by the MSS Configurator.

## Libero Designs and MSS configuration files:

The Icicle kit Libero design used with this project can be found [here](https://mi-v-ecosystem.github.io/redirects/repo-icicle-kit-reference-design).
The reference design includes the MSS Configurator source file **MPFS_ICICLE_MSS_baremetal.cfg**
and the software configuration output file **MPFS_ICICLE_MSS_baremetal.xml**

For all the boards supported with this project (including Icicle kit), the MSS
Configurator generated .cfg file and the .xml output generated from it are also
part of this SoftConsole project.

Please see **src/boards/\<my-board>/fpga_design/design_description>** directory
for each board.

## Adding a new board and configuring DDR :
Following sections describe adding a new board to the SoftConsole project.

At this stage it is assumed that you have created a new Libero design and MSS
Configuration for your board using Libero and MSS configurator tool and the **MPFS_ICICLE_MSS_baremetal.cfg**
and **MPFS_ICICLE_MSS_baremetal.xml** from this new design are available.

### Add new board under the boards directory

Create a new directory /\<my_board> under the src/boards directory. Use one of
the existing board directories (e.g icicle-kit-es) as a template, copying and renaming as appropriate.
	* src/boards/**\<my-board>**

The following table describes the folder heirarchy and the files that should be
present under your src/boards/**\<my-board>** folder.

| Boards/*my_board*/files    						| Description    |
| :-------------             					   	| :----------    |
| fpga_design/design_description/design.xml  	   	| Output generated by MSS Configurator. The SoftConsole project uses this file to generate header files used by MPFS HAL |
| fpga_design_config/*       					   	| Will Contain header files generated from design.xml when you build the SoftConsole project|
| platform_config/mpfs_hal_config/mss_sw_config.h  	| Embedded software configuration file required by MPFS-HAL |
| fpga_design/mss_configuration/design.cfg         	| MSS Configurator source file.|

 - Ensure that the build step to generate header files from the xml now uses the new
paths to /\<my_board> directories. [^configgen]

 - Ensure that these new paths are used in your project's include path. [^includepath]

## Updating DDR settings from a previous MSS Configurator release

This example project is tested with the MSS Configuration generated from
MSS Configurator **v2022.2**. If you are presently using a MSS configuration
generated with an earlier version of the MSS Configurator, you will need to
refresh the default values of your DDR configuration to use the currently
recommended default DDR settings.

To update the default DDR settings from an older .cfg version you must do the
following :

1. Open your current MSS configuration in the MSS Configurator **v2022.2**.
2. Make sure that the "DDR memory type" is chosen appropriately matching your DDR memory. E.g. LPDDR4
3. Click on "MSS_\<DDR-type>_default_configuration" preset and click "Apply". (e.g. "MSS_LPDDR4_default_configuration")
4. Make sure that there are no overwrites relating to DDR in your project's
   *boards/your_board/platform_config/mpfs_hal_config/mss_sw_config.h* file
   i.e. The DDR section should appear as shown below

   ```c
		/*
		 * DDR software options
		 */

		/*
		 * Debug DDR startup through a UART
		 * Comment out in normal operation. Useful for debug purposes in bring-up of DDR
		 * in a new board design.
		 * See the weak function setup_ddr_debug_port(mss_uart_instance_t * uart)
		 * If you need to edit this function, make a copy of the function without the
		 * weak declaration in your application code.
		 * */
		#define DEBUG_DDR_INIT
		#define DEBUG_DDR_RD_RW_FAIL
		//#define DEBUG_DDR_RD_RW_PASS
		//#define DEBUG_DDR_CFG_DDR_SGMII_PHY
		//#define DEBUG_DDR_DDRCFG

		#endif

   ```

3. Cross check the new configurations

### Enable DDR debugging
When bringing-up DDR on a new board or when migrating from older design to the latest
libero version, enabling the DDR debugging aid the process.

#### Enable DDR debug macros
1. In the mss_sw_config.h file uncomment the following defines so that the DDR
training debug information is printed on a UART terminal.

   ```c
   #define DEBUG_DDR_INIT
   #define DEBUG_DDR_RD_RW_FAIL
   ```

2. By default, debug info is printed to MMUART0. To change the UART,
add a definition in your application code (see e51.c in this project, modify if required):

   ```c
   mss_uart_instance_t *g_debug_uart= &g_mss_uart3_lo ;
   ```

   Also in your application code add the following function if you want to change the default MMUART used.

   ```c
   uint32_t setup_ddr_debug_port(mss_uart_instance_t * uart)
   {
        (void)mss_config_clk_rst(MSS_PERIPH_MMUART3,
								(uint8_t) MPFS_HAL_FIRST_HART,
								PERIPHERAL_ON);

        MSS_UART_init(uart,
					  MSS_UART_115200_BAUD,
					  MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
        return(0U);
    }
    ```

#### Compile and debug

1. Select the \<board>-LIM-DEBUG build configuration and compile
2. Connect a serial terminal emulator to COM0
3. Run the mpfs-hal-ddr-demo hw all-harts debug.launch debug launcher
4. Make sure that you see the following expected output on the UART COM port

#### Expected terminal emulator output

1. The following menu should appear if the DDR has trained successfully

  ```c
  This program is run from E51

  DDR test options:
  1  Show DDR training values
  2  Soft reset the MSS
  3  Read/write memory - DDR tests
  4  Display clock values
  5  Load DDR test pattern and run.
  b  Display MSS PLL registers

  Bootloader options:
  6  Load image to DDR using YMODEM
  7  Start U54_1 from DDR @0x80000000
  8  Start U54 1 & 2 from DDR @0x80000000
  9  Start U54 1, 2 & 3 from DDR @0x80000000
  a  Start all U54s from DDR @0x80000000

  Type 0  to show this menu again
  ```

## The mss_sw_config.h

The mss_sw_config.h in every build configuration of the SoftConsole project provides
the software configurations It configures such as the number of harts used by the software etc.

It also allows settings coming from the MSS Configurator generated input to be
overwritten for bringup or debugging purposes. If there are any previous overwrites
in the mss_sw_config.h file, then it can cause an issue when updating to the new
configurations from the latest MSS configuration (2022.2 and later), as updated
default settings from MSS configurator will will be overwritten by the values
present in the mss_sw_config.h. Recomendation is to migrate to use latest MSS
Configurator (2022.2 and later) and **avoid any DDR settings overwrites**.

For the latest DDR settings for the Icicle kit, refer to the **MSS LPDDR4 configuration**
section in the [Icicle kit reference design documentation ](https://mi-v-ecosystem.github.io/redirects/repo-icicle-kit-reference-design).

## UART configuration

On connecting Icicle kit J11 to the host PC, you should see four COM port
interfaces connected. This example project requires MMUART0. To use this
project the host PC must connect to the COM port interface0 using a terminal
emulator such as HyperTerminal or Putty configured as follows:

- 115200 baud
- 8 data bits
- 1 stop bit
- no parity
- no flow control

## Target hardware

This example project is targeted at a number of different boards
 - Icicle kit
 - DDR base board with DDR3 installed
 - DDR base board with DDR4 installed
 - IOF Max verification board ( LPDDR3 )
 - Peripheral Base board

The non-Icicle kit boards are internal Microchip boards but the settings related
to the DDR variants are a guide on how to configure your board if based on one
of those DDR technologies.

## Porting DDR configuration to Hart Software Service

To get DDR for your board running on the HSS, port the following elements from
your working bare metal project.

- The same DDR configuration from the MSS Configurator as used in Bare Metal
- Make sure that there are no unwanted DDR related overwrites in the mss_sw_config.h
file under hart-software-services/boards/\<my-board> directory.


## References

[PolarFire® FPGA and PolarFire SoC FPGA Memory Controller](https://onlinedocs.microchip.com/pr/GUID-FF8061A7-7A15-470F-A6F5-E733C24D85F0-en-US-1/index.html)  
[PolarFire® SoC MSS Technical Reference Manual](https://onlinedocs.microchip.com/pr/GUID-0E320577-28E6-4365-9BB8-9E1416A0A6E4-en-US-3/index.html)  
[SoftConsole Releases](https://www.microchip.com/en-us/products/fpgas-and-plds/fpga-and-soc-design-tools/soc-fpga/softconsole#Download%20Software)

[^configgen]: ### Update build step to generate header files from xml configurations
	Go to:

	 - SoftConsole project explorer
	 - \<project name\> -> Project properties -> C/C++ Build -> Settings
	 - Select appropriate Build configuration
	 - Select "Build steps" tab and change the contents of the "Command"

	 ```c
	 ${env_var:MACRO_PYTHON_BINARY_PATH_AND_EXECUTABLE} ../src/platform/soc_config_generator/mpfs_configuration_generator.py ../src/boards/<my-board>/fpga_design/design_description   ../src/boards/<my-board>
	 ```

[^includepath]: ### Update the project settings to change project's include path.
	Go to:

	 - SoftConsole project explorer
	 - \<project name\> -> Project properties -> C/C++ Build -> Settings
	 - Select appropriate Build configuration
	 - Select "Tool Settings"
	 - Select "GNU RISC-V Cross C Compiler -> includes

	 ```eclipse
	  "${workspace_loc:/${ProjName}/src/boards/<my-board>}"
	  "${workspace_loc:/${ProjName}/src/boards/<my-board>/platform_config}"
	 ```
