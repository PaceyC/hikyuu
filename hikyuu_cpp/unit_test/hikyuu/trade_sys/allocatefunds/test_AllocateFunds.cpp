/*
 * test_AllocateFunds.cpp
 *
 *  Created on: 2018-2-11
 *      Author: fasiondog
 */

#include "doctest/doctest.h"
#include <hikyuu/StockManager.h>
#include <hikyuu/trade_manage/crt/crtTM.h>
#include <hikyuu/trade_sys/system/crt/SYS_Simple.h>
#include <hikyuu/trade_sys/selector/crt/SE_Fixed.h>
#include <hikyuu/trade_sys/allocatefunds/crt/AF_EqualWeight.h>

using namespace hku;

/**
 * @defgroup test_AllocateFunds test_AllocateFunds
 * @ingroup test_hikyuu_trade_sys_suite
 * @{
 */

/** @par 检测点 */
TEST_CASE("test_AllocateFunds") {
    StockManager& sm = StockManager::instance();

    SEPtr se = SE_Fixed();
    AFPtr af = AF_EqualWeight();
    TMPtr tm = crtTM(Datetime(200101010000L), 100000);
    TMPtr subtm = crtTM(Datetime(200101010000L), 0);
    SYSPtr sys = SYS_Simple(subtm->clone());

    SystemList se_list;
    SystemList hold_list;
    SystemList ac_list;
    SystemWeightList sw_list;

    /** @arg 测试setTM */
    CHECK_UNARY(!af->getTM());
    af->setTM(tm);
    CHECK_UNARY(af->getTM());

    /** @arg 测试clone */
    AFPtr af2 = af->clone();
    CHECK_NE(af2.get(), af.get());
    CHECK_EQ(af2->name(), af->name());
    CHECK_EQ(af2->getTM(), af->getTM());

    /** @arg 出入的se_list、hold_list均为空  */
    CHECK_EQ(se_list.size(), 0);
    CHECK_EQ(hold_list.size(), 0);
    sw_list = af->_allocateWeight(Datetime(201802100000L), se_list);
    ac_list = af->getAllocatedSystemList(Datetime(201802100000L), se_list, hold_list);
    CHECK_EQ(sw_list.size(), 0);
    CHECK_EQ(ac_list.size(), 0);

    /** @arg 最大持仓系统数小于0 */
    af->setParam<int>("max_sys_num", -1);
    se->clear();
    tm->reset();
    sys->setTM(subtm->clone());
    se->addStock(sm["sh600000"], sys);
    se_list = se->getSelectedSystemList(Datetime(201802100000L));
    CHECK_EQ(se_list.size(), 1);

    sw_list = af->_allocateWeight(Datetime(201802100000L), se_list);
    ac_list = af->getAllocatedSystemList(Datetime(201802100000L), se_list, se_list);
    CHECK_EQ(sw_list.size(), 1);
    CHECK_EQ(sw_list[0].getSYS(), se_list[0]);
    CHECK_EQ(ac_list.size(), 0);
    CHECK_EQ(tm->currentCash(), 100000);

    /** @arg 最大持仓系统数为0 */
    af->setParam<int>("max_sys_num", 0);
    tm->reset();
    se->clear();
    sys->setTM(subtm->clone());
    se->addStock(sm["sh600000"], sys);
    se_list = se->getSelectedSystemList(Datetime(201802100000L));
    CHECK_EQ(se_list.size(), 1);

    sw_list = af->_allocateWeight(Datetime(201802100000L), se_list);
    ac_list = af->getAllocatedSystemList(Datetime(201802100000L), se_list, hold_list);
    CHECK_EQ(sw_list.size(), 1);
    CHECK_EQ(ac_list.size(), 0);
    CHECK_EQ(tm->currentCash(), 100000);
}

/** @} */
