# ===============================================================

MOC_HEADER = HeroQuestStartDialog.h \
             HeroQuestNewGameDialog.h \
             HeroQuestLevelWindow.h \
             HeroStatisticPane.h \
             DiceRollPane.h \
             DiceRollArea.h \
             ButtonPane.h \
             HintPane.h \
             Playground.h \
             DialogOkCancel.h \
             DialogOk.h \
             DialogTreasureCard.h \
             DialogOpenChest.h \
             DialogInventory.h \
             TreasureCardPane.h \
             SpellCardPane.h \
             UseInventoryItemButton.h \
             BuyItemButton.h \
             DialogGameOver.h \
             DialogLevelFinished.h \
             TreasureDescriptionPane.h \
             DialogLevelBriefing.h \
             DialogBuyEquipment.h \
             InventoryPane.h \
             EquipmentCardPane.h \
             EquipmentMarketPlacePane.h
             
# ===============================================================

ifneq ($(TARGET_MODE),RELEASE)
  ifneq ($(TARGET_MODE),DEBUG)
    $(error Set environment variable TARGET_MODE to either RELEASE or DEBUG.)
  endif
endif

# ===============================================================

ifeq (X$(QT_PATH),X)
  $(error Set environment variable QT_PATH, e.g. "e:/Programmieren/ProgrammierenCPP/Qt5/5.3/msvc2010_opengl")
endif

# ===============================================================

ifeq (X$(WINDOWS_UCRT_INCLUDE),X)
  $(error Set environment variable WINDOWS_UCRT_INCLUDE, e.g. \"C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt/x64\")
endif

ifeq (X$(WINDOWS_UM_INCLUDE),X)
  $(error Set environment variable WINDOWS_UM_INCLUDE, e.g. \"C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um/x64\")
endif

ifeq (X$(WINDOWS_SHARED_INCLUDE),X)
  $(error Set environment variable WINDOWS_SHARED_INCLUDE, e.g. \"C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/shared/x64\")
endif

ifeq (X$(WINDOWS_UCRT_LIBPATH),X)
  $(error Set environment variable WINDOWS_UCRT_LIBPATH, e.g. \"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x64\")
endif

ifeq (X$(WINDOWS_UM_LIBPATH),X)
  $(error Set environment variable WINDOWS_UM_LIBPATH, e.g. \"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64\")
endif

# ===============================================================

ifeq ($(TARGET_MODE),RELEASE)
  TARGET_DIR = release
else
  TARGET_DIR = debug
endif
TARGET = $(TARGET_DIR)/HeroQuest.exe
SOURCE_DIR = source
QT_LIBPATH = $(QT_PATH)"/lib"

# ===============================================================

# In the MOC_DIR we save moc sources and moc object files.
# In the RCC_DIR we save resource files.
MOC_DIR = $(TARGET_DIR)/moc
RCC_DIR = $(TARGET_DIR)/rcc
OBJ_DIR = $(TARGET_DIR)/objects
PDB_DIR = $(TARGET_DIR)/pdb

# ===============================================================

SOURCES = $(wildcard $(SOURCE_DIR)/*.cpp)

# moc-cpp files which will be created from moc-relevant headers
MOC_SOURCES = $(patsubst %.h,$(MOC_DIR)/moc-%.cpp,$(MOC_HEADER))

# Qt's rcc tool creates qrc_application.cpp from *.qrc
RESOURCES = $(wildcard $(SOURCE_DIR)/*.qrc)

# created from the qrc resource file
RCC_TARGET = $(RCC_DIR)/qrc_application.rcc


# to get OBJ, we take SOURCES and replace SOURCE_DIR by OBJ_DIR
OBJ = $(subst $(SOURCE_DIR)/,$(OBJ_DIR)/,$(patsubst %.cpp,%.obj,$(SOURCES)))

# to get MOC_OBJ, we take MOC_SOURCES and replace *.cpp by *.obj
MOC_OBJ = $(patsubst %.cpp,%.obj,$(MOC_SOURCES))

# ===============================================================

PRECOMPILE = moc.exe
RCCCOMPILE = rcc.exe

COMPILE = cl.exe
ADDITIONAL_INCLUDES = /I$(QT_PATH)/include /I$(WINDOWS_UCRT_INCLUDE) /I$(WINDOWS_UM_INCLUDE) /I$(WINDOWS_SHARED_INCLUDE)

# compile options explained:
#   "/std:c++17", // C++ 17 standard
#   "/c", // generate object file only (no exe)
#   "/Zi", // produce separate PDB file which contains debugging information
#   "/MD", // use the multithread-specific and DLL-specific version of the run-time library;
#          // defines _MT and _DLL and causes the compiler to place the library name MSVCRT.lib into the .obj file
#   "/nologo", // suppresses the display of the copyright banner when the compiler starts up
#   "/W3", // displays level 1, level 2, and level 3 (production quality) warnings
#   "/GL", // enable whole program optimization
#   "/EHsc", // exception handling: "s": enables standard C++ stack unwinding;
#            // "c": the compiler assumes that functions declared as extern "C" never throw a C++ exception
#   "/GS", // enable buffer security check (detects some buffer overruns)
#   "/Gy", // allow the compiler to put functions in a way that they can be optimized by the linker
#   "/fp:precise", // the compiler preserves the source expression ordering and rounding properties of floating-point code
#   "/Zc:wchar_t", // Parse wchar_t as a built-in type according to the C++ standard
#   "/Zc:forScope", //  let a for loop's initializer go out of scope after the for loop
#   "/Zc:__cplusplus", // enable the __cplusplus preprocessor macro to report an updated value for recent C++ language standards support
#   "/GR", // enable runtime type information
#   "/Gd", // specify the __cdecl calling convention for all functions except C++ member functions and functions that are marked __stdcall, __fastcall, or __vectorcall
#   "/analyze-", // turn off code analysis
#   "/DNOMINMAX", // NOMINMAX required for QDialog
COMPILE_OPTIONS = /std:c++17 /c /Zi /MD /nologo /W3 /GL /EHsc /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:__cplusplus /GR /Gd /analyze- /DNOMINMAX
# old: COMPILE_OPTIONS = /std:c++17 /c /Zi /MD /nologo /W3 /WX- /GL /Gm- /EHsc /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:__cplusplus /GR /Gd /analyze- /errorReport:queue /DNOMINMAX # NOMINMAX required for QDialog
ifeq ($(TARGET_MODE),RELEASE)
COMPILE_OPTIONS += /O2 /Oy- /Oi
else
COMPILE_OPTIONS += /Od
endif
DEFINE = /D "WIN32" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE"
ifeq ($(TARGET_MODE),RELEASE) 
  DEFINE += /D "NDEBUG"
else
  DEFINE += /D "DEBUG"
endif

LINK = link.exe
LINK_TARGET = /OUT:"$(TARGET)"
LINK_OPTIONS = /MACHINE:X64 /INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /ALLOWISOLATION /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /ERRORREPORT:QUEUE /DEBUG /LIBPATH:$(QT_LIBPATH) /LIBPATH:$(WINDOWS_UCRT_LIBPATH) /LIBPATH:$(WINDOWS_UM_LIBPATH)
LINK_LIBS = Qt6Cored.lib \
            Qt6Guid.lib \
            Qt6Widgetsd.lib \
            Qt6Multimediad.lib \
            winmm.lib \
            Psapi.lib \
            gdiplus.lib \
            OpenGL32.lib \
            Glu32.lib \
            Shlwapi.lib \
            kernel32.lib \
            user32.lib \
            gdi32.lib \
            winspool.lib \
            comdlg32.lib \
            advapi32.lib \
            shell32.lib \
            ole32.lib \
            oleaut32.lib \
            uuid.lib \
            odbc32.lib \
            odbccp32.lib 

# ===============================================================

# about Qt moc handling, read http://qt-project.org/doc/qt-4.8/moc.html
$(MOC_DIR)/moc-%.cpp:	$(SOURCE_DIR)/%.h
	$(PRECOMPILE) $< -o $@

# about Qt rcc handling, read http://doc.qt.io/qt-5/resources.html
$(RCC_TARGET):	$(RESOURCES)
	$(RCCCOMPILE) --binary --output $(RCC_TARGET) $(RESOURCES)

$(OBJ_DIR)/%.obj:	$(SOURCE_DIR)/%.cpp $(SOURCE_DIR)/*.h
	$(COMPILE) $(ADDITIONAL_INCLUDES) $(COMPILE_OPTIONS) $(DEFINE) $< /Fo$@ /Fd$(subst $(OBJ_DIR)/,$(PDB_DIR)/,$(patsubst %.obj,%.pdb,$@))

$(MOC_DIR)/%.obj:	$(MOC_DIR)/%.cpp $(SOURCE_DIR)/*.h
	$(COMPILE) $(ADDITIONAL_INCLUDES) $(COMPILE_OPTIONS) $(DEFINE) $< /Fo$@ /Fd$(subst $(MOC_DIR)/,$(PDB_DIR)/,$(patsubst %.obj,%.pdb,$@))
	
$(TARGET):	$(MOC_OBJ) $(RCC_OBJ) $(OBJ) $(RCC_TARGET)
	$(LINK) $(LINK_TARGET) $(LINK_OPTIONS) $(LINK_LIBS) $(MOC_OBJ) $(OBJ)

.PHONY: directories
directories:
	@mkdir -p $(TARGET_DIR)
	@mkdir -p $(MOC_DIR)
	@mkdir -p $(RCC_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(PDB_DIR)
	@mkdir -p $(EXECUTE_DIR)

all:	directories $(TARGET)
	cp $(TARGET) $(EXECUTE_DIR)/
	cp $(TARGET) $(RELEASE_DIR)/
	mkdir -p $(EXECUTE_DIR)/resources
	mkdir -p $(RELEASE_DIR)/resources
	cp $(RCC_DIR)/* $(EXECUTE_DIR)/resources
	cp $(RCC_DIR)/* $(RELEASE_DIR)/resources
	mkdir -p $(EXECUTE_DIR)/pdb
	cp $(PDB_DIR)/* $(EXECUTE_DIR)/pdb

clean:
	rm -f $(OBJ_DIR)/*.* $(PDB_DIR)/*.* $(MOC_DIR)/*.* $(RCC_DIR)/*.* $(TARGET) $(patsubst %.exe,%.pdb,$(TARGET))
	rm -rf $(EXECUTE_DIR)
