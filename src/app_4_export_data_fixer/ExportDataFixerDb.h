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
        string name_s;
        string name_ss;
        string name_vs;
        string spectralType;
    };

    class ExportDataFixerDb {
    public:
        explicit ExportDataFixerDb(const string&);

        void startQuery(const function<void(const FixerRow *)>&);
        void append(long long index, const string& name_s, const string& name_ss, const string& name_vs) const;
        void commit() const;

    private:
        shared_ptr<pqxx::connection> _dbReadConnection;
        shared_ptr<PostgresBatchUpdate> _batchAdder;
        const pqxx::zview _selectString = "SELECT index, name_s, name_ss, name_vs FROM simbad WHERE name_s IS NOT NULL OR name_ss IS NOT NULL OR name_vs IS NOT NULL";
        const string _upsertString = "INSERT INTO export_overrides (index, new_name_s, new_name_ss, new_name_vs) VALUES {} ON CONFLICT (index) DO UPDATE SET new_name_s=EXCLUDED.new_name_s, new_name_ss=EXCLUDED.new_name_ss, new_name_vs=EXCLUDED.new_name_vs;";
    };
}
