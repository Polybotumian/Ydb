#include <iostream>
#include "Ydb.h"

int main()
{
    ydb::Database db("myDb.json");
    std::cout << db.Path() << " - " << db.Name() << " - " << db.Extension() << '\n';
	try
	{
		db.Create("Dokuman_0", "Deneme");
		db.Get("Dokuman_0").Create("Dokuman_1", "Veri");
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
	std::cout << db.Count() << '\n';
	db.Save();
    return 0;
}