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
             DialogGameOver.h \
             TreasureDescriptionPane.h \
             DialogLevelBriefing.h
             
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

ifeq (X$(BOOST_PATH),X)
  $(error Set environment variable BOOST_PATH, e.g. "e:/Programmieren/ProgrammierenCPP/boost/boost_1_74_0")
endif

# ===============================================================

ifeq ($(TARGET_MODE),RELEASE)
  TARGET_DIR = release
else
  TARGET_DIR = debug
endif
TARGET = $(TARGET_DIR)/HeroQuest.exe
EXECUTE_DIR = execute
SOURCE_DIR = source
QT_LIBPATH = $(QT_PATH)"/lib"
BOOST_LIBPATH = $(BOOST_PATH)"/libs"

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
ADDITIONAL_INCLUDES = /I$(QT_PATH)/include /I$(BOOST_PATH)
COMPILE_OPTIONS = /c /Zi /MD /nologo /W3 /WX- /GL /Gm- /EHsc /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /GR /Gd /analyze- /errorReport:queue /DNOMINMAX # NOMINMAX required for QDialog
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
LINK_OPTIONS = /MACHINE:X86 /INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /ALLOWISOLATION /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /ERRORREPORT:QUEUE /DEBUG /LIBPATH:$(QT_LIBPATH)
LINK_LIBS = Qt5Cored.lib \
            Qt5Guid.lib \
            Qt5Widgetsd.lib \
            Qt5Multimediad.lib \
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
	mkdir -p $(EXECUTE_DIR)/resources
	cp $(RCC_DIR)/* $(EXECUTE_DIR)/resources
	mkdir -p $(EXECUTE_DIR)/pdb
	cp $(PDB_DIR)/* $(EXECUTE_DIR)/pdb

clean:
	rm -f $(OBJ_DIR)/*.* $(PDB_DIR)/*.* $(MOC_DIR)/*.* $(RCC_DIR)/*.* $(TARGET) $(patsubst %.exe,%.pdb,$(TARGET))
	rm -rf $(EXECUTE_DIR)
