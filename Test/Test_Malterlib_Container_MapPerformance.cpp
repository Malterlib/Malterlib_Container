// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Test/Test>

#include "../../Core/Source/Malterlib_Core_Misc.h"
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/containers/flat_set.hpp>
#include <boost/interprocess/containers/flat_map.hpp>

// This is disabled for GCC & clang due to a lookup issue with
// boost declaring std::pair and the compiler not finding the definition from <utlity>
#if !defined(DMibCompiler_GCC) && !defined(DCompiler_clang)

namespace NMib
{
	namespace NMisc
	{

		namespace Tests {
			using namespace NMib::NTime;
			using namespace NMib::NContainer;
			typedef CTimerMin TestTimer;
			class MapTester 
			{
				int m_nItems;
				bint m_bRandom;
				std::vector<std::pair<int,int>> m_Key_Data;
			private: // Measure
				template <typename Op>
				inline_never void MeasureForAllKeys(CTimerMin& Time,Op op) const
				{
					Time.f_Start();
					for(int i=0;i<10000;++i) { op(m_Key_Data[i].first); }
					Time.f_Stop();
				}
				template <typename Op>
				inline_never void MeasureForAllKeyValues(CTimerMin& Time,Op op) const
				{
					Time.f_Start();
					for(int i=0;i<10000;++i) { op(m_Key_Data[i].first,m_Key_Data[i].second); }
					Time.f_Stop();
				}
			private: // Generate test data
				template <class Map>
				Map getFilledMap() const
				{					
					Map map;
					for(int i=0;i<10000;++i) { map[m_Key_Data[i].first] = m_Key_Data[i].second; }
					return map;
				}
				template <class Set>
				Set getFilledSet() const
				{					
					Set set;
					for(int i=0;i<10000;++i) { set.insert(m_Key_Data[i].first); }
					return set;
				}
				template <>
				TCMap<int,int> getFilledMap<TCMap<int,int>>() const
				{					
					TCMap<int,int> map;
					for(int i=0;i<10000;++i) { map[m_Key_Data[i].first] = m_Key_Data[i].second; }
					return map;
				}
				template <>
				TCMap<int> getFilledSet<TCMap<int>>() const
				{					
					TCMap<int> set;
					for(int i=0;i<10000;++i) { set[m_Key_Data[i].first]; }
					return set;
				}
			private: // Operations
				template <class Set>
				void insertInSet(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Set set;
					TestTimer Time;
					MeasureForAllKeys(Time,[&](int key) { set.insert(key); });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				}
				template <class Map>
				void insertInMap(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Map set2;
					TestTimer Time;
					MeasureForAllKeyValues(Time,[&](int key,int value) { set2[key] = value; });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				}
				template <class Set>
				void accessInSet(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Set set2 = getFilledSet<Set>();
					TestTimer Time;

					bool found = true; // To prevent loop from being optimized away 
					MeasureForAllKeys(Time,[&](int key) { found = found && (set2.find(key) != set2.end()); });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				};
				template <class Map>
				void accessInMap(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Map set = getFilledMap<Map>();
					int found = 0; // To prevent loop from being optimized away 
					TestTimer Time;
					MeasureForAllKeys(Time,[&](int key) { found += set[key]; });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				};
				template <class Set>
				void eraseInSet(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Set set = getFilledSet<Set>();
					TestTimer Time;
					MeasureForAllKeys(Time,[&](int key) { set.erase(key); });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				}
				template <class Map>
				void eraseInMap(CTimerMin const& MalterlibTime) {
					fp_CheckInit();
					Map set = getFilledMap<Map>();
					TestTimer Time;
					MeasureForAllKeys(Time,[&](int key) { set.erase(key); });
					DMibTest(DMibExpr( Time / MalterlibTime) > DMibExpr(1.0));
				}

				void fp_CheckInit()
				{
					if (m_Key_Data.empty())
					{
						m_Key_Data.reserve(m_nItems);
						if (!m_bRandom) 
						{
							for(int i=0;i<m_nItems;++i) { m_Key_Data.push_back(std::make_pair(i,i)); }
						} 
						else 
						{
							NMisc::CRandomShiftRNG RandomRng;
							for(int i = 0; i < m_nItems; ++i) 
								m_Key_Data.push_back(std::make_pair(1 + (RandomRng.f_GetValue<int>() % (10000000-1)),1 + (RandomRng.f_GetValue<int>() % (10000000-1))));
						}
					}
				}
				
			public: // Test cases
				MapTester(int num_items,bool random) 
					: m_nItems(num_items)
					, m_bRandom(random)
				{
				}
				void insert_id_in_set() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib") 
					{
						fp_CheckInit();
						TCMap<int> set1;
						bint created;
						MeasureForAllKeys(MalterlibTime,[&](int key) { set1.f_Map(key,created); });
					};
					DMibTestSuite("std::set")           { this->insertInSet<std::set<int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_set") { this->insertInSet<std::unordered_set<int>>(MalterlibTime); };
					//DMibTestSuite("boost::flat_set")    { this->insertInSet<boost::interprocess::flat_set<int>>(MalterlibTime); };
				}
				void insert_id_in_map() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib") 
					{ 
						fp_CheckInit();
						TCMap<int,int> set1;
						MeasureForAllKeyValues(MalterlibTime,[&](int key,int value) { set1[key] = value; });
                    };
					DMibTestSuite("std::map")           { this->insertInMap<std::map<int,int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_map") { this->insertInMap<std::tr1::unordered_map<int,int>>(MalterlibTime); };
					//DMibTestSuite("boost::flat_map")    { this->insertInMap<boost::interprocess::flat_map<int,int>>(MalterlibTime); };
				}					
				void access_id_in_map() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib") 
					{ 
						fp_CheckInit();
						auto set = this->getFilledMap<TCMap<int,int>>();
						int found = 0;
						MeasureForAllKeys(MalterlibTime,[&](int key) {  found += set[key]; });
					};
					DMibTestSuite("std::map")           { this->accessInMap<std::map<int,int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_map") { this->accessInMap<std::unordered_map<int,int>>(MalterlibTime); };
					//DMibTestSuite("boost::flat_map")    { this->accessInMap<boost::interprocess::flat_map<int,int>>(MalterlibTime); };
				}
				void access_id_in_set() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib") 
					{
						fp_CheckInit();
						TCMap<int> set1;
						for(int i=0;i<10000;++i) { set1[m_Key_Data[i].first]; }

						bint found = true;
						MeasureForAllKeys(MalterlibTime,[&](int key) { found = found && !!set1.f_Exists(key); });
					};
					DMibTestSuite("std::set")           { this->accessInSet<std::set<int>>(MalterlibTime); };
					//DMibTestSuite("boost::flat_set")    { this->accessInSet<boost::interprocess::flat_set<int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_set") { this->accessInSet<std::tr1::unordered_set<int>>(MalterlibTime); };					
				}
				void erase_id_in_set() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib")
					{ 
						fp_CheckInit();
						auto set = this->getFilledSet<TCMap<int>>();
						MeasureForAllKeys(MalterlibTime,[&](int key) { set.f_Remove(key); });
					};
					DMibTestSuite("std::set")           { this->eraseInSet<std::set<int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_set") { this->eraseInSet<std::tr1::unordered_set<int>>(MalterlibTime); };					
					//DMibTestSuite("boost::flat_set")    { this->eraseInSet<boost::interprocess::flat_set<int>>(MalterlibTime); };
				}
				void erase_id_in_map() 
				{
					TestTimer MalterlibTime;
					DMibTestSuite("Malterlib") 
					{ 
						fp_CheckInit();
						auto set = this->getFilledMap<TCMap<int,int>>();
						MeasureForAllKeys(MalterlibTime,[&](int key) { set.f_Remove(key); });
					};
					DMibTestSuite("std::map")           { this->eraseInMap<std::map<int,int>>(MalterlibTime); };
					//DMibTestSuite("std::unordered_map") { this->eraseInMap<std::unordered_map<int,int>>(MalterlibTime); };
					//DMibTestSuite("boost::flat_map")    { this->eraseInMap<boost::interprocess::flat_map<int,int>>(MalterlibTime); };
				}
			};
			class CMapPerformance_Tests : public CTest
			{

			public: // Define test suites
				void f_Suite(MapTester&& mt)
				{
					DMibTestCategory("Set") 
					{
						DMibTestCategory("Insert") 
						{ 
							mt.insert_id_in_set(); 
						};
						DMibTestCategory("Access") 
						{ 
							mt.access_id_in_set(); 
						};
						DMibTestCategory("Erase")
						{ 
							mt.erase_id_in_set(); 
						};
					};
					DMibTestCategory("Map") 
					{
						DMibTestCategory("Insert") 
						{ 
							mt.insert_id_in_map(); 
						};
						DMibTestCategory("Access") 
						{ 
							mt.access_id_in_map(); 
						};
						DMibTestCategory("Erase")  
						{ 
							mt.erase_id_in_map(); 
						};
					};
				}
				void f_DoTests()
				{
					DMibTestCategory(CTestCategory("Performace") << CTestGroup("Performance"))
					{
						DMibTestCategory("Random Input")
						{
							f_Suite(MapTester(100000,true));
						};
						DMibTestCategory("Linear input")
						{
							f_Suite(MapTester(100000,false));
						};
					};
				}
			};
			DMibTestRegister(CMapPerformance_Tests, Malterlib::Container);
		}


	}
}
#endif
