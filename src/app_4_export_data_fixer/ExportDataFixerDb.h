#pragma once
#include <pqxx/pqxx>
#include "PostgresBatchUpdate.h"

using namespace std;
using namespace thxsoft::database::postgres;

namespace thxsoft::export_data_fixer_db
{
    struct FixerRow
    {
        long long index;
        string name1;
        string name2;
        string name3;
        string name4;
        string spectralType;
    };

    class ExportDataFixerDb {
    public:
        explicit ExportDataFixerDb(const string&);

        void startQuery(const function<void(const FixerRow *)>&);
        void append(long long index, const string& name1, const string& name2, const string& name3) const;
        void commit() const;

    private:
        shared_ptr<pqxx::connection> _dbReadConnection;
        shared_ptr<PostgresBatchUpdate> _batchAdder;
        const pqxx::zview _selectString = "SELECT index, name1, name2, name3, name4 FROM simbad WHERE name1 IS NOT NULL";
        const string _upsertString = "INSERT INTO export_overrides (index, new_name1, new_name2, new_name3) VALUES {} ON CONFLICT (index) DO UPDATE SET new_name1=EXCLUDED.new_name1, new_name2=EXCLUDED.new_name2, new_name3=EXCLUDED.new_name3;";
    };
}
