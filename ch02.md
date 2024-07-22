# 2 The FreeRTOS Kernel Distribution

## 2.1 Introduction

To help users orientate themselves with the FreeRTOS kernel
files and directories, this chapter:

- Provides a top-level view of the FreeRTOS directory structure.
- Describes the source files required by any particular FreeRTOS project.
- Introduces the demo applications.
- Provides information on how to create a new FreeRTOS project.

The description here relates only to the official FreeRTOS distribution.
The examples that come with this book use a slightly different
organization.


## 2.2 Understanding the FreeRTOS Distribution

### 2.2.1 Definition: FreeRTOS Port

FreeRTOS can be built with approximately twenty different compilers and can
run on more than forty different processor architectures. Each supported
combination of compiler and processor is called a FreeRTOS port.


### 2.2.2 Building FreeRTOS

FreeRTOS is a library that provides multi-tasking capabilities to what
would otherwise be a single-threaded, bare-metal application.

FreeRTOS is supplied as a set of C source files. Some source files are
common to all ports, while others are specific to a port. Building the
source files as part of your project makes the FreeRTOS API available to
your application. A demo application that can be used as a reference is
provided for each official FreeRTOS port. The demo application is
pre-configured to build the correct source files and include the correct
header files.

At the time of its creation, each demo application built 'out of the
box' with no compiler errors or warnings. Please use the FreeRTOS
support forums (<https://forums.FreeRTOS.org>) to let us know if
subsequent changes to the build tools mean that this is no longer the case.
Section 2.3 describes the demo applications.


### 2.2.3 FreeRTOSConfig.h

Constants defined in a header file called FreeRTOSConfig.h configure the
kernel. Do not include FreeRTOSConfig.h directly in your source files!
Instead, include FreeRTOS.h, which will include FreeRTOSConfig.h at the
appropriate time.

FreeRTOSConfig.h is used to tailor the FreeRTOS kernel for use in a
specific application. For example, FreeRTOSConfig.h contains constants
such as `configUSE_PREEMPTION` which defines whether
FreeRTOS uses co-operative or pre-emptive scheduling[^1].

[^1]: Section 4.13 describes scheduling algorithms.

FreeRTOSConfig.h tailors FreeRTOS for a specific application, so it
should be located in a directory that is part of the application, not in
a directory that contains the FreeRTOS source code.

The main FreeRTOS distribution contains a demo application for every
FreeRTOS port, and every demo application has its own FreeRTOSConfig.h
file. It is recommended to start with, then adapt, the FreeRTOSConfig.h
used by the demo application provided for the FreeRTOS port you use
rather than create the file from scratch.

The FreeRTOS reference manual and <https://www.freertos.org/a00110.html>
describe the constants that appear in FreeRTOSConfig.h. It is
not necessary to include all the constants in FreeRTOSConfig.h—many get a
default value if omitted.


### 2.2.4 Official Distributions

Individual FreeRTOS libraries, including the kernel, are available from
their own Github repository and as a zip file archive. The ability to
obtain individual libraries is convenient when using FreeRTOS in
production code. However, it is better to download the main FreeRTOS
distribution to get started as that contains both libraries and example
projects.

The main distribution contains source code for all the FreeRTOS
libraries, all the FreeRTOS kernel ports, and project files for all the
FreeRTOS demo applications. Do not be put off by the number of files!
Applications only require a small subset.

Use <https://github.com/FreeRTOS/FreeRTOS/releases/latest> to download a
zip file containing the latest distribution. Alternatively, use one of
the following Git commands to clone the main distribution from GitHub,
including individual libraries sub-moduled from their respective Git
repositories:

* * *
```
git clone https://github.com/FreeRTOS/FreeRTOS.git --recurse-submodules

git clone git@github.com:FreeRTOS/FreeRTOS.git --recurse-submodules
```
* * *


Figure 2.1 shows the first and second-level directories of the FreeRTOS
distribution:

<a name="fig2.1" title="Figure 2.1 Top level directories within the FreeRTOS distribution"></a>

* * *
```
FreeRTOS
│ │
│ ├─Source  Contains the FreeRTOS kernel source files
│ │
│ └─Demo    Contains pre-configured and port specific FreeRTOS kernel demo projects
│
FreeRTOS-Plus
│
├─Source    Contains source code for other FreeRTOS and ecosystem libraries
│
└─Demo      Contains demo projects for other FreeRTOS and ecosystem libraries
```
***Figure 2.1*** *Top level directories within the FreeRTOS distribution*
* * *

The distribution only contains one copy of the FreeRTOS kernel source
files; all the demo projects expect to find the kernel source files in
the FreeRTOS/Source directory, and may not build if the directory
structure is changed.


### 2.2.5 FreeRTOS Source Files Common to All Ports

tasks.c and list.c implement the core FreeRTOS kernel functionality and
are always required. They are located directly in the FreeRTOS/Source
directory, as shown in Figure 2.2. The same directory also contains the
following optional source files:

-  **queue.c**

   queue.c provides both queue and semaphore services, as described later
   in this book. queue.c is nearly always required.

-  **timers.c**

   timers.c provides software timer functionality, as described later in
   this book. It only needs to be built if the application uses software
   timers.

-  **event\_groups.c**

   event\_groups.c provides event group functionality, as described later
   in this book. It only needs to be built if the application uses event
   groups.

-  **stream\_buffer.c**

   stream\_buffer.c provides both stream buffer and message buffer
   functionality, as described later in this book. It only needs to
   be built if the application uses
   stream or message buffers.

-  **croutine.c**

   croutine.c implements the FreeRTOS co-routine functionality. It only
   needs to be built if the application uses co-routines. Co-routines are
   intended for use on very small microcontrollers, are rarely used now.
   They are, therefore, no longer maintained, and their use is not
   recommended for new designs. Co-routines are not described in this
   book.


<a name="fig2.2" title="Figure 2.2 Core FreeRTOS source files within the FreeRTOS directory tree"></a>

* * *
```
FreeRTOS
│
└─Source
    │
    ├─tasks.c         FreeRTOS source file - always required
    ├─list.c          FreeRTOS source file - always required
    ├─queue.c         FreeRTOS source file - nearly always required
    ├─timers.c        FreeRTOS source file - optional
    ├─event_groups.c  FreeRTOS source file – optional
    ├─stream_buffer.c FreeRTOS source file - optional
    └─croutine.c      FreeRTOS source file – optional and no longer maintained
```
***Figure 2.2*** *Core FreeRTOS source files within the FreeRTOS directory tree*
* * *

It is recognized that the file names used in the zip file distribution
may cause namespace clashes, as many projects will already use files
with the same names. Users can change the file names if necessary, but
the names cannot change in the distribution, as doing so will break
compatibility with existing users' projects as well as FreeRTOS-aware
development tools.


### 2.2.6 FreeRTOS Source Files Specific to a Port

The FreeRTOS/Source/portable directory contains source files specific to a FreeRTOS
port. The portable directory is arranged as a hierarchy, first by
compiler, then by processor architecture. Figure 2.3 shows the hierarchy.

To run FreeRTOS on a processor with architecture '*architecture*' using
compiler '*compiler*', in addition to the core FreeRTOS source
files, you must also build the files located in the
FreeRTOS/Source/portable/\[*compiler*\]/\[*architecture*\] directory.

As described in Chapter 3, Heap Memory Management, FreeRTOS also
considers heap memory allocation to be part of the portable layer. If
`configSUPPORT_DYNAMIC_ALLOCATION` is set to 0, then do not include a heap
memory allocation scheme in your project.

FreeRTOS provides example heap allocation schemes in the
FreeRTOS/Source/portable/MemMang directory. If FreeRTOS is configured to
use dynamic memory allocation, it is necessary to either include one of
the heap implementation source files from that directory in your project, or
provide your own implementation.

> **! Do not include more than one of the example heap allocation implementations in your project.**


<a name="fig2.3" title="Figure 2.3 Port specific source files within the FreeRTOS directory tree"></a>

* * *
```
FreeRTOS
│
└─Source
    │
    └─portable Directory containing all port specific source files
        │
        ├─MemMang Directory containing the alternative heap allocation source files
        │
        ├─[compiler 1] Directory containing port files specific to compiler 1
        │   │
        │   ├─[architecture 1] Contains files for the compiler 1 architecture 1 port
        │   ├─[architecture 2] Contains files for the compiler 1 architecture 2 port
        │   └─[architecture 3] Contains files for the compiler 1 architecture 3 port
        │
        └─[compiler 2] Directory containing port files specific to compiler 2
            │
            ├─[architecture 1] Contains files for the compiler 2 architecture 1 port
            ├─[architecture 2] Contains files for the compiler 2 architecture 2 port
            └─[etc.]
```
***Figure 2.3*** *Port specific source files within the FreeRTOS directory tree*
* * *

### 2.2.7 Include Paths

FreeRTOS requires the inclusion of three directories in the compiler's
include path. These are:

1. The path to the core FreeRTOS kernel header files, FreeRTOS/Source/include.

2. The path to the source files specific to the FreeRTOS port in use,
   FreeRTOS/Source/portable/\[*compiler*\]/\[*architecture*\].

3. The path to the correct FreeRTOSConfig.h header file.


### 2.2.8 Header Files

A source file that uses the FreeRTOS API must include FreeRTOS.h,
followed by the header file that contains the prototype for the API
function—either task.h, queue.h, semphr.h, timers.h,
event_groups.h, stream_buffer.h, message_buffer.h or croutine.h. Do not
explicitly include any other FreeRTOS header files—FreeRTOS.h automatically includes
FreeRTOSConfig.h.


## 2.3 Demo Applications

Each FreeRTOS port comes with at least one demo application that, at the
time of its creation, built 'out of the box' with no compiler errors or
warnings. Please use the FreeRTOS support forums
(<https://forums.FreeRTOS.org>) to let us know if subsequent changes to
the build tools mean that is no longer the case.

> **Cross Platform Support**: FreeRTOS is developed and tested on Windows,
> Linux and MacOS systems and with a variety of toolchains, both embedded and
> traditional.  However, occasionally build errors can appear due to differences
> of version or a missed test.  Please use the FreeRTOS support forum
> (<https://forums.FreeRTOS.org>) to alert us of any such errors.

Demo applications have several purposes:

- To provide an example of a working and pre-configured project, with
  the correct files included, and the correct compiler options set.
- To allow 'out of the box' experimentation with minimal setup or
  prior knowledge.
- To demonstrate how to use FreeRTOS APIs.
- As a base from which real applications can be created.
- To stress test the kernel's implementation.

Each demo project is located in a unique sub-directory under the
FreeRTOS/Demo directory. The sub-directory's name indicates the port to
which the demo project relates.

The FreeRTOS.org website contains a page for each demo application. The
web page includes information on:

- How to locate the project file for the demo within the FreeRTOS directory
  structure.
- The hardware or emulator the project is configured to use.
- How to set up the hardware to run the demo.
- How to build the demo.
- The demo's expected behaviour.

All demo projects create a subset of the 'common demo tasks', the
implementations of which are in the FreeRTOS/Demo/Common/Minimal
directory. The common demo tasks exist to demonstrate how to use the
FreeRTOS API and test FreeRTOS kernel ports—they do not implement any
particular useful functionality.

Many demo projects can also be configured to create a simple 'blinky'
style starter project that typically creates two RTOS tasks and one
queue.

Every demo project includes a file called main.c that contains the
`main()` function, which creates the demo application tasks before starting
the FreeRTOS kernel. See the
comments within the individual main.c files for information specific to
that demo.

<a name="fig2.4" title="Figure 2.4 The demo directory hierarchy"></a>

* * *
```
FreeRTOS
    │
    └─Demo          Directory containing all the demo projects
        │
        ├─[Demo x]  Contains the project file that builds demo 'x'
        ├─[Demo y]  Contains the project file that builds demo 'y'
        ├─[Demo z]  Contains the project file that builds demo 'z'
        └─Common    Contains files that are built by all the demo applications
```
***Figure 2.4*** *The demo directory hierarchy*
* * *


## 2.4 Creating a FreeRTOS Project

### 2.4.1 Adapting One of the Supplied Demo Projects

Every FreeRTOS port comes with at least one pre-configured demo
application. It is recommended to create new projects by adapting one of
these existing projects to ensure the new project has the correct files
included, the correct interrupt handlers installed, and the correct
compiler options set.

To create a new application from an existing demo project:

1. Open the supplied demo project and ensure it builds and executes as
   expected.

2. Remove the source files that implement the demo tasks, which are
   files located in the Demo/Common directory.

3. Delete all the function calls within `main()`, except
   `prvSetupHardware()` and `vTaskStartScheduler()`, as shown in Listing 2.1.

4. Verify that the project still builds.

When you follow these steps you create a project that includes the correct
FreeRTOS source files, but does not define any functionality.


<a name="list2.1" title="Listing 2.1 The template for a new main() function"></a>


```c
int main( void )
{
    /* Perform any hardware setup necessary. */
    prvSetupHardware();

    /* --- APPLICATION TASKS CAN BE CREATED HERE --- */

    /* Start the created tasks running. */
    vTaskStartScheduler();

    /* Execution will only reach here if there was insufficient heap to
       start the scheduler. */
    for( ;; );
    return 0;
}
```
***Listing 2.1*** *The template for a new main() function*



### 2.4.2 Creating a New Project from Scratch

As already mentioned, it is recommended to create new projects from an
existing demo project. If this is not desirable, then use the following
procedure to create a new project:

1. Using your chosen toolchain, create a new project that does not yet
   include any FreeRTOS source files.

1. Ensure the new project builds, downloads to your target hardware,
   and executes.

1. Only when you are sure you already have a working project, add the
   FreeRTOS source files detailed in Table 1 to the project.

1. Copy the `FreeRTOSConfig.h` header file used by the demo project and
   provided for the port in use into your new project directory.

1. Add the following directories to the path the project will search to
   locate header files:

   - FreeRTOS/Source/include
   - FreeRTOS/Source/portable/\[*compiler*\]/\[*architecture*\] (where
     \[*compiler*\] and \[*architecture*\] are correct for your chosen port)
   - The directory containing the `FreeRTOSConfig.h` header file

1. Copy the compiler settings from the relevant demo project.

1. Install any FreeRTOS interrupt handlers that might be necessary. Use
   the web page that describes the port in use and the demo project
   provided for the port in use as a reference.

<a name="tbl1" title="Table 1 FreeRTOS source files to include in the project"></a>

* * *
| File                            | Location                     |
|---------------------------------|------------------------------|
| tasks.c                         | FreeRTOS/Source              |
| queue.c                         | FreeRTOS/Source              |
| list.c                          | FreeRTOS/Source              |
| timers.c                        | FreeRTOS/Source              |
| event\_groups.c                 | FreeRTOS/Source              |
| stream\_buffer.c                | FreeRTOS/Source              |
| All C and assembler files       | FreeRTOS/Source/portable/\[compiler\]/\[architecture\] |
| heap\_n.c                       | FreeRTOS/Source/portable/MemMang, where n is either 1, 2, 3, 4 or 5 |

***Table 1*** *FreeRTOS source files to include in the project*
* * *

**Note on heap memory:**
 If `configSUPPORT_DYNAMIC_ALLOCATION` is 0 then do not include a heap memory
 allocation scheme in your project. Else include a heap memory allocation scheme
 in your project, either one of the heap\_*n*.c files, or one provided by
 yourself. Refer to Chapter 3, Heap Memory Management, for more information.


## 2.5 Data Types and Coding Style Guide

### 2.5.1 Data Types

Each port of FreeRTOS has a unique portmacro.h header file that contains
(amongst other things) definitions for two port-specific data types:
`TickType_t` and `BaseType_t`. The following list describes the macro or
typedef used and the actual type:

- `TickType_t`

  FreeRTOS configures a periodic interrupt called the tick interrupt.

  The number of tick interrupts that have occurred since the FreeRTOS
  application started is called the *tick count*. The tick count is
  used as a measure of time.

  The time between two tick interrupts is called the *tick period*.
  Times are specified as multiples of tick periods.

  `TickType_t` is the data type used to hold the tick count value, and to
  specify times.

  `TickType_t` can be an unsigned 16-bit type, an unsigned 32-bit
  type, or an unsigned 64-bit type, depending on the setting of `configTICK_TYPE_WIDTH_IN_BITS`
  in FreeRTOSConfig.h. The setting of `configTICK_TYPE_WIDTH_IN_BITS` is architecture
  dependent. FreeRTOS ports will also check if this setting is valid.

  Using a 16-bit type can greatly improve efficiency on 8-bit and
  16-bit architectures, but severely limits the maximum block time that
  can be specified in FreeRTOS API calls. There is no reason to use a
  16-bit `TickType_t` type on a 32-bit or 64-bit architecture.

  Previous use of `configUSE_16_BIT_TICKS` has been replaced by `configTICK_TYPE_WIDTH_IN_BITS` to support
  tick counts greater than 32-bits. New designs should use `configTICK_TYPE_WIDTH_IN_BITS`
  instead of `configUSE_16_BIT_TICKS`.

   <a name="tbl2" title="Table 2 TickType_t data type and the configTICK_TYPE_WIDTH_IN_BITS configuration"></a>

   * * *
   | configTICK\_TYPE\_WIDTH\_IN\_BITS | 8-bit architectures | 16-bit architectures | 32-bit architectures | 64-bit architectures |
   | --- | --- | --- | --- | --- |
   | TICK\_TYPE\_WIDTH\_16_BITS | uint16\_t	| uint16\_t	| uint16\_t	| N/A |
   | TICK\_TYPE\_WIDTH\_32_BITS | uint32\_t	| uint32\_t	| uint32\_t	| N/A |
   | TICK\_TYPE\_WIDTH\_64_BITS | N/A | N/A | uint64\_t | uint64\_t |

   ***Table 2*** *TickType_t data type and the configTICK_TYPE_WIDTH_IN_BITS configuration*
   * * *

- `BaseType_t`

  This is always defined as the most efficient data type for the
  architecture. Typically, this is a 64-bit type on a 64-bit architecture,
  a 32-bit type on a 32-bit architecture, a 16-bit type on a 16-bit
  architecture, and an 8-bit type on an 8-bit architecture.

  `BaseType_t` is generally used for return types that take only a very
  limited range of values, and for `pdTRUE`/`pdFALSE` type Booleans.


*List of port specific data types used by FreeRTOS*


### 2.5.2 Variable Names

Variables are prefixed with their type: 'c' for `char`, 's' for `int16_t`
(short), 'l' for `int32_t` (long), and 'x' for `BaseType_t` and any other
non-standard types (structures, task handles, queue handles, etc.).

If a variable is unsigned, it is also prefixed with a 'u'. If a variable
is a pointer, it is also prefixed with a 'p'. For example, a variable of
type `uint8_t` will be prefixed with 'uc', and a variable of type pointer
to char (`char *`) will be prefixed with 'pc'.


### 2.5.3 Function Names

Functions are prefixed with both the type they return and the file they
are defined within. For example:

- v**Task**PrioritySet() returns a *v*oid and is defined within **tasks**.c.
- x**Queue**Receive() returns a variable of type *BaseType_t* and is defined within **queue**.c.
- pv**Timer**GetTimerID() returns a *p*ointer to *v*oid and is defined within **timers**.c.

File scope (private) functions are prefixed with 'prv'.


### 2.5.4 Formatting

Tabs are used in some demo applications where one tab is always set to
equal four spaces. The kernel no longer uses tabs.


### 2.5.5 Macro Names

Most macros are written in upper case, and prefixed with lower case
letters that indicate where the macro is defined. Table 3 provides a
list of prefixes.

<a name="tbl3" title="Table 3 Macro prefixes"></a>

* * *
| Prefix                                       | Location of macro definition   |
|----------------------------------------------|--------------------------------|
| port (for example, `portMAX_DELAY`)          | `portable.h` or `portmacro.h`  |
| task (for example, `taskENTER_CRITICAL()`)   | `task.h`                       |
| pd (for example, `pdTRUE`)                   | `projdefs.h`                   |
| config (for example, `configUSE_PREEMPTION`) | `FreeRTOSConfig.h`             |
| err (for example, `errQUEUE_FULL`)           | `projdefs.h`                   |

***Table 3*** *Macro prefixes*
* * *

Note that the semaphore API is written almost entirely as a set of
macros, but follows the function naming convention, rather than the
macro naming convention.

The macros defined in Table 4 are used throughout the FreeRTOS source code.

<a name="tbl4" title="Table 4 Common macro definitions"></a>

* * *
| Macro        | Value |
|--------------|-------|
| `pdTRUE`     | 1     |
| `pdFALSE`    | 0     |
| `pdPASS`     | 1     |
| `pdFAIL`     | 0     |

***Table 4*** *Common macro definitions*
* * *


### 2.5.6 Rationale for Excessive Type Casting

The FreeRTOS source code compiles with many different compilers, many of
which differ in how and when they generate warnings. In particular,
different compilers want casting used in different ways. As a result,
the FreeRTOS source code contains more type casting than would normally
be warranted.
