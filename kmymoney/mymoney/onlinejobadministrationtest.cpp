#include "onlinejobadministrationtest.h"

#include <QtTest/QTest>

#include "onlinejobadministration.h"
#include "mymoney/mymoneyfile.h"
#include "mymoney/storage/mymoneyseqaccessmgr.h"
#include "sepaonlinetransfer.h"

QTEST_MAIN(onlineJobAdministrationTest)

void onlineJobAdministrationTest::initTestCase()
{
    file = MyMoneyFile::instance();
    storage = new MyMoneySeqAccessMgr;
    file->attachStorage(storage);

    try {
        MyMoneyAccount account = MyMoneyAccount();
        account.setName( "Test Account" );
        account.setAccountType( MyMoneyAccount::Savings );
        MyMoneyAccount asset = file->asset();
        MyMoneyFileTransaction transaction;
        file->addAccount(account , asset );
        accountId = account.id();
        transaction.commit();
    } catch (MyMoneyException* ex) {
        QFAIL( qPrintable("Unexpected exception " + ex->what()) );
        delete ex;
    }
}

void onlineJobAdministrationTest::cleanupTestCase()
{
    file->detachStorage(storage);
    delete storage;
}

void onlineJobAdministrationTest::init()
{
  qDeleteAll(onlineJobAdministration::instance()->m_onlineTasks);
  onlineJobAdministration::instance()->m_onlineTasks.clear();
}

void onlineJobAdministrationTest::getSettings()
{
}

void onlineJobAdministrationTest::registerOnlineTask()
{
  sepaOnlineTransfer *task = new sepaOnlineTransfer;
  onlineJobAdministration::instance()->registerOnlineTask(task);
  QCOMPARE(onlineJobAdministration::instance()->m_onlineTasks.count(), 1);
  QVERIFY(onlineJobAdministration::instance()->m_onlineTasks.value( task->taskName() ));
}
