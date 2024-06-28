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
        string name6;
        string name7;
        string name8;
        string spectralType;
    };

    class ExportDataFixerDb {
    public:
        explicit ExportDataFixerDb(const string&);

        void startQuery(const function<void(const FixerRow *)>&);
        void append(long long index, const string& name6, const string& name7, const string& name8) const;
        void commit() const;

    private:
        shared_ptr<pqxx::connection> _dbReadConnection;
        shared_ptr<PostgresBatchUpdate> _batchAdder;
        const pqxx::zview _selectString = "SELECT index, name6, name7, name8 FROM simbad WHERE name6 IS NOT NULL OR name7 IS NOT NULL OR name8 IS NOT NULL";
        const string _upsertString = "INSERT INTO export_overrides (index, new_name6, new_name7, new_name8) VALUES {} ON CONFLICT (index) DO UPDATE SET new_name6=EXCLUDED.new_name6, new_name7=EXCLUDED.new_name7, new_name8=EXCLUDED.new_name8;";
    };
}
