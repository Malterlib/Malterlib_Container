// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Exception>

using namespace NMib::NContainer;
using namespace NMib::NStr;
using namespace NMib;

#ifdef DCompiler_clang
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif

namespace
{
	template <typename t_CRegistry>
	struct TCMoveTest
	{
		static void fs_DoTest()
		{
			t_CRegistry Registry;
			t_CRegistry *pChild = Registry.f_SetValue("Test1", "Test1");
			DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
			DMibTest(DMibExpr(Registry.f_DebugIsValid()));
			{
				DMibTestPath("Constructor");
				t_CRegistry Registry2(fg_Move(Registry));
				DMibMovedFromValid(Registry);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move from"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move to"));
				Registry = fg_Move(Registry2);
				DMibMovedFromValid(Registry2);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move from"));
			}
			{
				DMibTestPath("Constructor child");
				t_CRegistry Registry2(fg_Move(*pChild));
				DMibMovedFromValid(*pChild);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetThisValue()) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(0u));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move from"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move to"));
				*pChild = fg_Move(Registry2);
				DMibMovedFromValid(Registry2);
				DMibTest(DMibExpr(Registry2.f_GetThisValue()) == DMibExpr(""));
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move from"));
			}
			{
				DMibTestPath("Assign");
				t_CRegistry Registry2;
				Registry2 = fg_Move(Registry);
				DMibMovedFromValid(Registry);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move from"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move to"));
				Registry = fg_Move(Registry2);
				DMibMovedFromValid(Registry2);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move from"));
			}
			{
				DMibTestPath("Assign child");
				t_CRegistry Registry2;
				t_CRegistry *pChild2 = Registry2.f_SetValue("Test2", "Test2");
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr("Test2"));

				*pChild2 = fg_Move(*pChild);

				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move from"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move to"));
				*pChild = fg_Move(*pChild2);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move from"));
			}
			{
				DMibTestPath("Assign child already existst");
				t_CRegistry Registry2;
				t_CRegistry *pChild2 = Registry2.f_SetValue("Test2", "Test2");
				[[maybe_unused]] t_CRegistry *pChild3 = Registry2.f_SetValue("Test1", "Test3");
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test3"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr("Test2"));

				*pChild2 = fg_Move(*pChild);

				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				if constexpr (t_CRegistry::mc_bSupportForceCreate)
					DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(2u));
				else
					DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move from"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move to"));
				*pChild = fg_Move(*pChild2);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr(""));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("After move to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("After move from"));
			}
		}
	};

	template <typename t_CRegistry>
	struct TCCopyTest
	{
		static void fs_DoTest()
		{
			{
				DMibTestPath("Copy Assignment");
				t_CRegistry Registry1;
				Registry1.f_SetValue("Test1", "Test1");
				DMibTest(DMibExpr(Registry1.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				t_CRegistry Registry2;
				Registry2.f_SetValue("Test2", "Test2");
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr("Test2"));
				t_CRegistry Registry3;
				Registry3.f_SetValue("Test3", "Test3");
				DMibTest(DMibExpr(Registry3.f_GetValue("Test3", "")) == DMibExpr("Test3"));
				DMibTest(DMibExpr(Registry1) != DMibExpr(Registry2)) (ETestFlag_NoValues);
				DMibTest(DMibExpr(Registry3) != DMibExpr(Registry2)) (ETestFlag_NoValues);
				DMibTest(DMibExpr(Registry3) != DMibExpr(Registry1)) (ETestFlag_NoValues);
				Registry1 = Registry2;
				DMibTest(DMibExpr(Registry1.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				DMibTest(DMibExpr(Registry1) == DMibExpr(Registry2)) (ETestFlag_NoValues);
				Registry2 = Registry3;
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry3.f_DebugIsValid()) && DMibExpr("Copy from"));
				DMibTest(DMibExpr(Registry2) != DMibExpr(Registry1)) (ETestFlag_NoValues);
				Registry1 = Registry1;
				DMibTest(DMibExpr(Registry1.f_DebugIsValid()) && DMibExpr("Copy same"));
				DMibTest(DMibExpr(Registry1) == DMibExpr(Registry1)) (ETestFlag_NoValues);

				t_CRegistry Registry4;
				Registry4.f_SetValue("Test3", "Test3");
				Registry4.f_SetValue("Test3", "Test3");
				DMibTest(DMibExpr(Registry4.f_GetValue("Test3", "")) == DMibExpr("Test3"));
				Registry3 = Registry4;
				DMibTest(DMibExpr(Registry3.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry3) == DMibExpr(Registry4)) (ETestFlag_NoValues);
			}
			t_CRegistry Registry;
			[[maybe_unused]] t_CRegistry *pChild = Registry.f_SetValue("Test1", "Test1");
			DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
			{
				DMibTestPath("Constructor");
				t_CRegistry Registry2((Registry));
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy from"));
				{
					DMibTestPath("Assign");
					Registry = (Registry2);
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
			}
			{
				DMibTestPath("Constructor child");
				t_CRegistry *pChild = Registry.f_SetValue("Test1", "Test1");
				t_CRegistry Registry2((*pChild));
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetThisValue()) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(0u));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy from"));
				{
					DMibTestPath("Assign");
					*pChild = (Registry2);
					DMibTest(DMibExpr(Registry2.f_GetThisValue()) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
			}
			{
				DMibTestPath("Assign");
				t_CRegistry Registry2;
				Registry2 = (Registry);
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
				DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
				DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy from"));
				{
					DMibTestPath("X");
					Registry = (Registry2);
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
			}
			{
				DMibTestPath("Assign child");
				t_CRegistry Registry2;
				t_CRegistry *pChild = Registry.f_SetValue("Test1", "Test1"); //
				t_CRegistry *pChild2 = Registry2.f_SetValue("Test2", "Test2");
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr("Test2"));

				*pChild2 = (*pChild);

				{
					DMibTestPath("X");
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr(""));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
				{
					DMibTestPath("X2");
					*pChild = (*pChild2);
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
			}
			{
				DMibTestPath("Assign child already existst");
				t_CRegistry Registry;
				t_CRegistry Registry2;
				t_CRegistry *pChild = Registry.f_SetValue("Test1", "Test1"); //
				t_CRegistry *pChild2 = Registry2.f_SetValue("Test2", "Test2");
				[[maybe_unused]] t_CRegistry *pChild3 = Registry2.f_SetValue("Test1", "Test3");
				DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test3"));
				DMibTest(DMibExpr(Registry2.f_GetValue("Test2", "")) == DMibExpr("Test2"));

				*pChild2 = (*pChild);

				{
					DMibTestPath("X");
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
				if constexpr (t_CRegistry::mc_bSupportForceCreate)
					DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(2u));
				else
					DMibTest(DMibExpr(Registry2.f_GetChildren().f_GetLen()) == DMibExpr(1u));
				*pChild = (*pChild2);
				{
					DMibTestPath("X2");
					DMibTest(DMibExpr(Registry.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry2.f_GetValue("Test1", "")) == DMibExpr("Test1"));
					DMibTest(DMibExpr(Registry.f_DebugIsValid()) && DMibExpr("Copy to"));
					DMibTest(DMibExpr(Registry2.f_DebugIsValid()) && DMibExpr("Copy from"));
				}
			}
		}
	};


	class CMalterlibRegistry_Tests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("Basic")
			{
				CRegistry Registry;

				CStr Temp = "Test { Test Test0 Test Test2 Test3 \"Abra\\\"Kadabra\"}";
				Registry.f_ParseStr(Temp);
				CStr Temp2 = Registry.f_GenerateStr();
				CRegistry Registry2;
				Registry2.f_ParseStr(Temp2);

				DMibTest(DMibExpr(Registry == Registry2));
			};

			DMibTestCategory("Preserve comments")
			{
				DMibTestSuite("Compact no comments")
				{
					CRegistryPreserveWhitespace Registry;

					CStr RegistryStr = "Test{\tTest Test0\tTest Test2\tTest3 \"Abra\\\"Kadabra\"}";
					Registry.f_ParseStr(RegistryStr);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					CRegistry Registry2;
					Registry2.f_ParseStr(RegistryStrGenerated);
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);
					DMibTest(DMibExpr(Registry == Registry2));
				};
				DMibTestSuite("Compact comments")
				{
					CRegistryPreserveWhitespace Registry;

					CStr RegistryStr = " /*Comm1*/ Test /*Comm2*/ { /*Comm3*/ Test /*Comm4*/ Test0 /*Comm5*/ Test /*Comm6*/ Test2 /*Comm7*/ Test3 /*Comm8*/ \"Abra\\\"Kadabra\"/*Comm8*/ }/*Comm9*/ ";
					Registry.f_ParseStr(RegistryStr);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					CRegistry Registry2;
					Registry2.f_ParseStr(RegistryStrGenerated);
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);
					DMibTest(DMibExpr(Registry == Registry2));
				};
				DMibTestSuite("Comments")
				{
					CRegistryPreserveWhitespace Registry;
					CStr RegistryStr =
					"// Comment before root\r\n"
					"1st_Root RootValue // Comment at root\r\n"
					"{ // Comment after children start\r\n"
					"\tValue1 Value1 // Comment after Value1\r\n"
					"\tValue2 /*Comment in middle for value 2*/ Value2\r\n"
					"/*Comment before Value3*/\tValue3 Value3\r\n"
					"/*Comment before Value4*/\tValue4 /*Comment in middle for value 4*/ Value4 // Comment after value3\r\n"
					"\t/*Multi line block comment\r\n"
					"\tMulti line block comment\r\n"
					"\tMulti line block comment\r\n"
					"\tMulti line block comment\r\n"
					"\tMulti line block comment*/\r\n"
					"\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"\t// Multi line comment\r\n"
					"/* Comment before children end */ } // Comment after children end\r\n"
					"End EndValue\r\n"
					"// Comment after end\r\n"
					;
					RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);
					CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";
					//NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
					Registry.f_ParseStr(RegistryStr, TmpFile);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);

					CRegistryPreserveWhitespace Registry2 = Registry;
					CStr RegistryStrGenerated2 = Registry2.f_GenerateStr();
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated2)) (ETestFlag_NoValues);
				};
				DMibTestSuite("No comments")
				{
					CRegistryPreserveWhitespace Registry;
					CStr RegistryStr =
					"1st_Root RootValue\r\n"
					"{\r\n"
					"\tValue1 Value1\r\n"
					"\tValue2 Value2\r\n"
					"\tValue3 Value3\r\n"
					"\tValue4 Value4\r\n"
					"}\r\n"
					"End EndValue\r\n";

					RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);

					CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";

					//NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
					Registry.f_ParseStr(RegistryStr, TmpFile);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);

					CRegistryPreserveWhitespace Registry2 = Registry;
					CStr RegistryStrGenerated2 = Registry2.f_GenerateStr();
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated2)) (ETestFlag_NoValues);
				};
				DMibTestSuite("Presevre interface")
				{
					CRegistryPreserveWhitespace Registry;
					CStr RegistryStr =
					"1st_Root RootValue\r\n"
					"{\r\n"
					"\tValue1 Value1\r\n"
					"\tValue2 Value2 // After Value 2\r\n"
					"\tValue3 Value3\r\n"
					"\tValue4 Value4\r\n"
					"}\r\n"
					"End EndValue\r\n";
					RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);
					CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";
					//NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
					Registry.f_ParseStr(RegistryStr, TmpFile);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);

					CRegistryPreserveWhitespace *pValue2 = Registry.f_GetChild("1st_Root/Value2");
					DMibTest(DMibExpr(pValue2) && DMibExpr(pValue2->f_GetWhiteSpace(ERegistryWhiteSpaceLocation_After)) == DMibExpr(" // After Value 2" DMibNewLine)) (ETestFlag_NoValues);

					CRegistryPreserveWhitespace Registry2 = Registry;
					CStr RegistryStrGenerated2 = Registry2.f_GenerateStr();
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated2)) (ETestFlag_NoValues);

					CRegistryPreserveWhitespace *pValue3 = Registry.f_GetChild("1st_Root/Value3");
					DMibTest(DMibExpr(pValue3));
					if (pValue3)
						pValue3->f_SetWhiteSpace(ERegistryWhiteSpaceLocation_After, " // After Value 3" DMibNewLine);
					if (pValue3)
						pValue3->f_SetWhiteSpace(ERegistryWhiteSpaceLocation_Between, "/* Middle */");
					if (pValue3)
						pValue3->f_SetWhiteSpace(ERegistryWhiteSpaceLocation_BeforeKey, "\t/* Before */");

					CStr RegistryStrResult =
					"1st_Root RootValue\r\n"
					"{\r\n"
					"\tValue1 Value1\r\n"
					"\tValue2 Value2 // After Value 2\r\n"
					"\t/* Before */Value3/* Middle */Value3 // After Value 3\r\n"
					"\tValue4 Value4\r\n"
					"}\r\n"
					"End EndValue\r\n";
					RegistryStrResult = RegistryStrResult.f_Replace("\r\n", DMibNewLine);
					CStr RegistryStrGenerated3 = Registry.f_GenerateStr();
					DMibTest(DMibExpr(RegistryStrResult) == DMibExpr(RegistryStrGenerated3)) (ETestFlag_NoValues);

				};
				DMibTestSuite("Corner cases")
				{
					CRegistryPreserveWhitespace Registry;
					CStr RegistryStr =
					"/* blaha\r\n"
					"haha */1st_Root RootValue\r\n"
					"/* blaha\r\n"
					"haha */{\r\n"
					"/* blaha\r\n"
					"haha */\tValue1 Value1\r\n"
					"\tValue2 Value2 // After Value 2\r\n"
					"\tValue3 Value3\r\n"
					"\tValue4 Value4\r\n"
					"/* blaha\r\n"
					"haha */}\r\n"
					"End EndValue\r\n";
					RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);
					CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";
					//NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
					Registry.f_ParseStr(RegistryStr, TmpFile);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					if (RegistryStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);

				};
				DMibTestSuite("Line breaks")
				{
					CRegistryPreserveWhitespace Registry;
					CStr RegistryStr =
						"Key0\\Second Value0\\Second\r\n"
						"Key1\\   Second Value1\\   Second\r\n"
						"\"Key2\"\\\"Second\"\r\n"
						"\"Value2\"\\\"Second\"\r\n"
						"Key3\\\"Second\"\r\n"
						"Value3\\\"Second\"\r\n"
						"\"Key4\"\\Second\r\n"
						"\"Value4\"\\Second\r\n"
						"\"Key5\"Second\r\n"
						"\"Value5\"Second\r\n"
						"\"Key6\\r\\n\"\\\r\n"
						"\"Second\\r\\n\"\\\r\n\"\"\r\n"
						"\"Value6\\r\\n\"\\\r\n"
						"\"Second\\r\\n\"\\\r\n\"\"\r\n"
						"XSub\r\n"
						"{\r\n"
						"\t\"Key7\\r\\n\"\\\r\n"
						"\t\"Second\\r\\n\"\\\r\n\t\"\"\r\n"
						"\t\"Value7\\r\\n\"\\\r\n"
						"\t\"Second\\r\\n\"\\\r\n\t\"\"\r\n"
						"}\r\n"
						"XSub1\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\n\"\\\r\n"
						"\t        \"Second\\r\\n\"\\\r\n\t        \"\"\r\n"
						"}\r\n"
						"XSub2\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\n\"\\\r\n"
						"\t        \"Second\\r\\n\"\\\r\n\t        \"\"// Comment!\r\n"
						"}\r\n"
						"XSub3\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\nSecond\\r\\n\"// Comment!\r\n"
						"}\r\n"
						"XSub4\r\n"
						"{\r\n"
						"\tKeyTest /* Mega! */\"Value7\\r\\n\"\\\r\n"
						"\t                   \"Second\\r\\n\"\\\r\n"
						"\t                   \"\"// Comment!\r\n"
						"}\r\n"
						"XSub5\r\n"
						"{\r\n"
						"\tKeyTest /* Mega!\r\n"
						"				*/\"Value7\\r\\n\"\\\r\n"
						"				  \"Second\\r\\n\"\\\r\n"
						"				  \"\"// Comment!\r\n"
						"}\r\n"
						;
					RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);
					CStr RegistryGenerateStr =
						"Key0Second Value0Second\r\n"
						"Key1Second Value1Second\r\n"
						"\"Key2Second\"\r\n"
						"\"Value2Second\"\r\n"
						"\"Key3Second\"\r\n"
						"\"Value3Second\"\r\n"
						"\"Key4Second\"\r\n"
						"\"Value4Second\"\r\n"
						"\"Key5Second\"\r\n"
						"\"Value5Second\"\r\n"
						"\"Key6\\r\\n\"\\\r\n"
						"\"Second\\r\\n\"\\\r\n\"\"\r\n"
						"\"Value6\\r\\n\"\\\r\n"
						"\"Second\\r\\n\"\\\r\n\"\"\r\n"
						"XSub\r\n"
						"{\r\n"
						"\t\"Key7\\r\\n\"\\\r\n"
						"\t\"Second\\r\\n\"\\\r\n\t\"\"\r\n"
						"\t\"Value7\\r\\n\"\\\r\n"
						"\t\"Second\\r\\n\"\\\r\n\t\"\"\r\n"
						"}\r\n"
						"XSub1\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\n\"\\\r\n"
						"\t        \"Second\\r\\n\"\\\r\n\t        \"\"\r\n"
						"}\r\n"
						"XSub2\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\n\"\\\r\n"
						"\t        \"Second\\r\\n\"\\\r\n\t        \"\"// Comment!\r\n"
						"}\r\n"
						"XSub3\r\n"
						"{\r\n"
						"\tKeyTest \"Value7\\r\\n\"\\\r\n"
						"\t        \"Second\\r\\n\"\\\r\n\t        \"\"// Comment!\r\n"
						"}\r\n"
						"XSub4\r\n"
						"{\r\n"
						"\tKeyTest /* Mega! */\"Value7\\r\\n\"\\\r\n"
						"\t                   \"Second\\r\\n\"\\\r\n"
						"\t                   \"\"// Comment!\r\n"
						"}\r\n"
						"XSub5\r\n"
						"{\r\n"
						"\tKeyTest /* Mega!\r\n"
						"				*/\"Value7\\r\\n\"\\\r\n"
						"				  \"Second\\r\\n\"\\\r\n"
						"				  \"\"// Comment!\r\n"
						"}\r\n"
						;
					RegistryGenerateStr = RegistryGenerateStr.f_Replace("\r\n", DMibNewLine);
					CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";
//					NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
					Registry.f_ParseStr(RegistryStr, TmpFile);
					CStr RegistryStrGenerated = Registry.f_GenerateStr();
					if (RegistryGenerateStr != RegistryStrGenerated)
						NMib::NSys::fg_Debug_DiffStrings(RegistryGenerateStr, RegistryStrGenerated);
					DMibTest(DMibExpr(RegistryGenerateStr) == DMibExpr(RegistryStrGenerated)) (ETestFlag_NoValues);

				};
				DMibTestSuite("Exceptions")
				{

					auto DoTest = [&] (CStr const &_String, const ch8 *_pDesc, const ch8 *_pDescDisambiguate)
					{
						CStr Category;
						if (*_pDescDisambiguate)
							Category = CStr::CFormat("{} {}") << _pDesc << _pDescDisambiguate;
						else
							Category = _pDesc;
						CStr Exception = CStr::CFormat(DMibPFileLineFormat " {}") << "" << 1 << _pDesc;
						fg_StrReplaceChar(Category, '/', ' ');
						Category = Category.f_Replace("\\", "Escape");
						{
							DMibTestPath(Category);
							CRegistryPreserveWhitespace Registry;
							DMibExpectException(Registry.f_ParseStr(_String), DMibErrorInstance(Exception));
						};
					};

					DoTest("Key value /* Comment not ending", "No end found for block comment", "");
					DoTest("{ }", "Children without key not supported", "");
					DoTest("Key value {} {}", "You cannot specify two child sections for one key", "");
					DoTest("key value key", "Mismatching key/value pairs within scope", "(root)");
					DoTest("key value {key value key}", "Mismatching key/value pairs within scope", "(children)");
					DoTest("Key value { ", "No matching '}' found", "");
					DoTest("Key value { key { }", "No matching '}' found", "(recursive)");
					DoTest("Key \"Value", "No matching end for escaped text (\")", "");
					DoTest("Key \"Value\r\n", "No matching end for escaped text (\") before end of line", "");
					DoTest("Key \"V\\alue\"", "Invalid escape character", "");
					DoTest("Key Value\\", "\\ needs to be followed with a continuation of the text", "(eof)");
					DoTest("Key Value\\{", "\\ needs to be followed with a continuation of the text", "({)");
					DoTest("Key Value\\}", "\\ needs to be followed with a continuation of the text", "(})");
					DoTest("Key Value\\//", "\\ needs to be followed with a continuation of the text", "(comment)");
					DoTest("Key Value\\/*", "\\ needs to be followed with a continuation of the text", "(multicommment)");
					DoTest("Key {} }", "No matching '{' found", "");
				};
				DMibTestSuite("Copy")
				{

					{
						DMibTestPath("Single");
						TCCopyTest<NMib::NContainer::TCRegistry<NStr::CStr, NStr::CStr>>::fs_DoTest();
					}
					{
						DMibTestPath("Multi");
						TCCopyTest<CRegistry>::fs_DoTest();
					}
					{
						DMibTestPath("Preserve");
						TCCopyTest<CRegistryPreserveWhitespace>::fs_DoTest();
					}
				};
				DMibTestSuite("Move")
				{

					{
						DMibTestPath("Single");
						TCMoveTest<TCRegistry<CStr, CStr>>::fs_DoTest();
					}
					{
						DMibTestPath("Multi");
						TCMoveTest<CRegistry>::fs_DoTest();
					}
					{
						DMibTestPath("Preserve");
						TCMoveTest<CRegistryPreserveWhitespace>::fs_DoTest();
					}
				};
				DMibTestSuite("Found bugs")
				{
					{
						DMibTestPath("1");
						CRegistryPreserveWhitespace Registry;
						CStr RegistryStr =
							"Key \r\n"
							"{\r\n"
							"\tKey\r\n"
							"\t{\r\n"
							"\t}\r\n"
							"\tValue2 Value2 // After Value 2\r\n"
							"}\r\n"
							;
						RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);

						CStr TmpFile = NSys::NFile::fg_GetTemporaryDirectory() + "/NoFile.ir";
						//NMib::NFile::CFile::fs_WriteStringToFile(TmpFile, RegistryStr);
						Registry.f_ParseStr(RegistryStr, TmpFile);
						CStr RegistryStrGenerated = Registry.f_GenerateStr();

						DMibTest(!(DMibExpr(TCThrowsException<NMib::NException::CException>()) == DMibLExpr(Registry.f_ParseStr(RegistryStrGenerated, TmpFile))));
					}

					{
						DMibTestPath("2");
						// Problem: Strings containing comment markers (//, /*) and no spaces were not escaped, so after writing them to a string reading back would fail as they would be parsed as comments. Doh!
						CStr RegStr;
						CStr ValueThatLooksLikeACommentButIsNot1 = "//Perforce/Depot/Path/Maybe";
						CStr ValueThatLooksLikeACommentButIsNot2 = "SomethingWithA//InTheMiddle";
						CStr ValueThatLooksLikeACommentButIsNot3 = "SomethingWithA/*BlockCommentAtTheEnd*/";


						{
							CRegistryPreserveWhitespace Registry;
							Registry.f_SetValue("Key1", ValueThatLooksLikeACommentButIsNot1);
							Registry.f_SetValue("Key2", ValueThatLooksLikeACommentButIsNot2);
							Registry.f_SetValue("Key3", ValueThatLooksLikeACommentButIsNot3);
							RegStr = Registry.f_GenerateStr();
							// DMibDTraceRaw(RegStr.f_GetStr());
						}
						{
							CRegistryPreserveWhitespace Reg;
							Reg.f_ParseStr(RegStr);

							DMibTest( DMibExpr(Reg.f_GetValue("Key1", "")) == DMibExpr(ValueThatLooksLikeACommentButIsNot1));
							DMibTest( DMibExpr(Reg.f_GetValue("Key2", "")) == DMibExpr(ValueThatLooksLikeACommentButIsNot2));
							DMibTest( DMibExpr(Reg.f_GetValue("Key3", "")) == DMibExpr(ValueThatLooksLikeACommentButIsNot3));
						}
					}
					{
						DMibTestPath("3");
						CStr RegistryStr =
							"\tKey Value // Comment\r\n";
						RegistryStr = RegistryStr.f_Replace("\r\n", DMibNewLine);
						{
							CRegistryPreserveWhitespace Registry;
							Registry.f_ParseStr(RegistryStr);

							DMibTest( DMibExpr(Registry.f_GetValue("Key", "")) == DMibExpr("Value"));
						}

						{
							CRegistry Registry;
							Registry.f_ParseStr(RegistryStr);

							DMibTest( DMibExpr(Registry.f_GetValue("Key", "")) == DMibExpr("Value") && DMibExpr(2));
						}

					}

					{
						DMibTestPath("2013-06-26");
						CStr RegistryStr = "Key0/Key1 Value";
						CRegistry Registry;
						Registry.f_ParseStr(RegistryStr);

						DMibTest( !DMibExpr(Registry.f_GetChildNoPath("Key0")));
						DMibTest( !DMibExpr(Registry.f_GetChild("Key0/Key1")));
						DMibTest( DMibExpr(Registry.f_GetChildNoPath("Key0/Key1")));
						DMibTest( DMibExpr(Registry.f_GetValueNoPath("Key0/Key1", "")) == DMibExpr("Value"));
						CStr ResultStr = Registry.f_GenerateStr();
						CStr ExpectedStr = "Key0/Key1 Value" DMibNewLine;

						DMibTest( DMibExpr(ResultStr) == DMibExpr(ExpectedStr))(ETestFlag_NoValues);
						if (ResultStr != ExpectedStr)
							NMib::NSys::fg_Debug_DiffStrings(ExpectedStr, ResultStr);
					}
				};
			};
		}
	};

	DMibTestRegister(CMalterlibRegistry_Tests, Malterlib::Container);
}
