#pragma once
#include <memory>
#include <QSqlDatabase>
#include "IDbPresenter.h"
#include "IConnectionProvider.h"

namespace views
{
    class ISqlView;
}

namespace presenters
{
    class MainPresenter : QObject, IDbPresenter, IConnectionProvider
    {
    public:
        MainPresenter();
        ~MainPresenter();
        virtual const Tables& GetTables() const override;
        virtual void PrepareInsert(const QString& tableName) override;
        virtual void DeleteSelected(const std::vector<int>& rows, const QString& tableName) override;
        virtual void Refresh(const QString& tableName) override;
        virtual bool Connect(const QString& host, int port, const QString& dbName,
                             const QString& username, const QString& password) override;
        void Start();

    private:
        std::unique_ptr<views::ISqlView> m_sqlView;
        QSqlDatabase m_db;
        Tables m_tables;
    };
}

