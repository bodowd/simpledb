#include "jdbc/embeddeddriver.hpp"
#include "jdbc/embeddedstatement.hpp"
#include "server/simpledb.hpp"
#include <exception>
#include <iostream>
#include <memory>
namespace simpledb {
void CreateStudentDB() {
  std::string filename = "students";
  auto db = std::make_unique<EmbeddedDriver>();
  try {

    auto conn = std::unique_ptr<EmbeddedConnection>(db->Connect(filename));
    auto stmt = std::unique_ptr<EmbeddedStatement>(conn->CreateStatement());

    std::string s = "create table STUDENT(SId int, SName varchar(10), MajorId "
                    "int, GradYear int)";

    stmt->ExecuteUpdate(s);

    std::cout << "Created STUDENT table" << std::endl;

    s = "create index majorid on STUDENT(MajorId)";
    stmt->ExecuteUpdate(s);
    std::cout << "Index majorid on STUDENT(MajorId) created" << std::endl;

    s = "insert into STUDENT(SId, SName, MajorId, GradYear) values ";
    std::vector<std::string> studVals = {
        "(1, 'joe', 10, 2021)", "(2, 'amy', 20, 2020)", "(3, 'max', 10, 2022)",
        "(4, 'sue', 20, 2022)", "(5, 'bob', 30, 2020)", "(6, 'kim', 20, 2020)",
        "(7, 'art', 30, 2021)", "(8, 'pat', 20, 2019)", "(9, 'lee', 10, 2021)"};
    for (int i = 0; i < static_cast<int>(studVals.size()); i++) {
      stmt->ExecuteUpdate(s + studVals[i]);
    }
    std::cout << "STUDENT records inserted." << std::endl;

    s = "create table DEPT(DId int, DName varchar(8))";
    stmt->ExecuteUpdate(s);
    std::cout << "Table DEPT created." << std::endl;

    s = "insert into DEPT(DId, DName) values ";
    std::vector<std::string> deptVals = {"(10, 'compsci')", "(20, 'math')",
                                         "(30, 'drama')"};
    for (int i = 0; i < static_cast<int>(deptVals.size()); i++) {
      stmt->ExecuteUpdate(s + deptVals[i]);
    }
    std::cout << "DEPT records inserted." << std::endl;

    s = "create table COURSE(CId int, Title varchar(20), DeptId int)";
    stmt->ExecuteUpdate(s);
    std::cout << "Table COURSE created" << std::endl;

    s = "insert into COURSE(CId, Title, DeptId) values ";
    std::vector<std::string> courseVals = {
        "(12, 'db systems', 10)", "(22, 'compilers', 10)",
        "(32, 'calculus', 20)",   "(42, 'algebra', 20)",
        "(52, 'acting', 30)",     "(62, 'elocution', 30)"};

    for (int i = 0; i < static_cast<int>(courseVals.size()); i++) {
      stmt->ExecuteUpdate(s + courseVals[i]);
    }
    std::cout << "COURSE records inserted." << std::endl;

    s = "create table SECTION(SectId int, CourseId int, Prof varchar(8), "
        "YearOffered int)";
    stmt->ExecuteUpdate(s);
    std::cout << "Table SECTION created." << std::endl;

    s = "insert into SECTION(SectId, CourseId, Prof, YearOffered) values ";
    std::vector<std::string> sectVals = {
        "(13, 12, 'turing', 2018)", "(23, 12, 'turing', 2019)",
        "(33, 32, 'newton', 2019)", "(43, 32, 'einstein', 2017)",
        "(53, 62, 'brando', 2018)"};

    for (int i = 0; i < static_cast<int>(sectVals.size()); i++) {
      stmt->ExecuteUpdate(s + sectVals[i]);
    }
    std::cout << "SECTION records inserted." << std::endl;

    s = "create table ENROLL(EId int, StudentId int, SectionId int, Grade "
        "varchar(2))";
    stmt->ExecuteUpdate(s);
    std::cout << "Table ENROLL created." << std::endl;

    s = "create index studentid on ENROLL(StudentId)";
    stmt->ExecuteUpdate(s);
    std::cout << "Index studentid on ENROLL(StudentId) created" << std::endl;

    s = "insert into ENROLL(EId, StudentId, SectionId, Grade) values ";
    std::vector<std::string> enrollVals = {
        "(14, 1, 13, 'A')",  "(24, 1, 43, 'C' )", "(34, 2, 43, 'B+')",
        "(44, 4, 33, 'B' )", "(54, 4, 53, 'A' )", "(64, 6, 53, 'A' )"};

    for (int i = 0; i < static_cast<int>(enrollVals.size()); i++) {
      stmt->ExecuteUpdate(s + enrollVals[i]);
    }

    std::cout << "ENROLL records inserted." << std::endl;
  } catch (std::exception &e) {
    std::cout << "ERROR: " << e.what() << std::endl;
  }
}

void StudentMajors() {
  std::string qry = "";
  qry += "select SName, DName ";
  qry += "from DEPT, STUDENT ";
  qry += "where MajorId = Did";

  std::string filename = "students";
  auto db = std::make_unique<EmbeddedDriver>();
  try {
    auto conn = std::unique_ptr<EmbeddedConnection>(db->Connect(filename));
    auto stmt = std::unique_ptr<EmbeddedStatement>(conn->CreateStatement());
    auto result_set = stmt->ExecuteQuery(qry);
    std::cout << "Name\tMajor" << std::endl;
    while (result_set->Next()) {
      std::string sName = result_set->GetString("sname");
      std::string dName = result_set->GetString("dname");
      std::cout << sName << "\t" << dName << std::endl;
    }
    result_set->Close();
  } catch (std::exception &e) {
    std::cout << "ERROR: " << e.what() << std::endl;
  }
}

} // namespace simpledb

int main() {
  simpledb::CreateStudentDB();
  simpledb::StudentMajors();
}
