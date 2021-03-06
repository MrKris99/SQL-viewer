#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include "MainPresenter.h"
#include "Views/ConnectDialog.h"
#include "Views/MainView.h"

presenters::MainPresenter::MainPresenter()
    : m_db(QSqlDatabase::addDatabase("QMYSQL"))
{

}

presenters::MainPresenter::~MainPresenter()
{
}

const presenters::Tables& presenters::MainPresenter::GetTables() const
{
    return m_tables;
}

void presenters::MainPresenter::ExecuteSql(const QString& query)
{
    QSqlQuery q(query);
    if (q.isSelect())
    {
        QSqlQueryModel* const model = new QSqlQueryModel(this);
        model->setQuery(q);
        m_sqlView->ShowSelectionResult(model);
    }

    if (!q.exec())
    {
        m_sqlView->AddLog(q.lastError().text());
    }
    else
    {
        m_sqlView->AddLog("Successfully executed query.");
    }
}

void presenters::MainPresenter::PrepareInsert(const QString& /*tableName*/)
{

}

void presenters::MainPresenter::DeleteSelected(const std::vector<int>& rows, const QString& tableName)
{
    std::for_each(rows.cbegin(), rows.cend(), [this, &tableName](int row)
    {
        m_tables[tableName]->removeRow(row);
    });
    this->Refresh(tableName);
}

void presenters::MainPresenter::Refresh(const QString& tableName)
{
    static_cast<QSqlTableModel*>(m_tables[tableName])->select();
}

bool presenters::MainPresenter::Connect(const ConnectionOptions& opt, const QString& password)
{
    m_db.setHostName(opt.host);
    m_db.setPort(opt.port);
    m_db.setDatabaseName(opt.db);
    m_db.setUserName(opt.username);
    m_db.setPassword(password);

    const bool isSuccess = m_db.open();

    if (isSuccess)
    {
        QSqlQuery query;
        query.exec("show tables");

        QStringList tables;
        while(query.next())
        {
            tables.append(query.value(0).toStringList());
        }

        foreach (const QString& table, tables)
        {
            QSqlTableModel* model = new QSqlTableModel(this, m_db);
            model->setTable(table);
            model->select();
            m_tables.emplace(table, model);
        }

        m_sqlView.reset(new views::MainView(this, opt));
        m_sqlView->Start();
    }

    return isSuccess;
}

void presenters::MainPresenter::Start()
{
    views::ConnectDialog* dialog = new views::ConnectDialog(this);
    dialog->show();
}
