/** Copyright 2008, 2009, 2010, 2011, 2012 Roland Olbricht
*
* This file is part of Overpass_API.
*
* Overpass_API is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Overpass_API is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Overpass_API.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DE__OSM3S___OVERPASS_API__STATEMENTS__UNARY_OPERATORS_H
#define DE__OSM3S___OVERPASS_API__STATEMENTS__UNARY_OPERATORS_H


#include "../data/tag_store.h"
#include "../data/utils.h"
#include "evaluator.h"
#include "statement.h"

#include <map>
#include <string>
#include <vector>


class Evaluator_Prefix_Operator : public Evaluator
{
public:
  Evaluator_Prefix_Operator(int line_number_);
  virtual void add_statement(Statement* statement, string text);
  virtual void execute(Resource_Manager& rman) {}
  
  virtual string get_result_name() const { return ""; }
  
  virtual std::pair< std::vector< Set_Usage >, uint > used_sets() const;
  virtual std::vector< std::string > used_tags() const;
  
  virtual Eval_Task* get_task(const Prepare_Task_Context& context);
  
  virtual std::string process(const std::string& rhs_result) const = 0;
    
protected:
  Evaluator* rhs;
};


struct Unary_Eval_Task : public Eval_Task
{
  Unary_Eval_Task(Eval_Task* rhs_, Evaluator_Prefix_Operator* evaluator_) : rhs(rhs_), evaluator(evaluator_) {}
  ~Unary_Eval_Task() { delete rhs; }
  
  virtual std::string eval(const std::string* key) const;
  
  virtual std::string eval(const Node_Skeleton* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Attic< Node_Skeleton >* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Way_Skeleton* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Attic< Way_Skeleton >* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Relation_Skeleton* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Attic< Relation_Skeleton >* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Area_Skeleton* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;
  virtual std::string eval(const Derived_Skeleton* elem,
      const std::vector< std::pair< std::string, std::string > >* tags, const std::string* key) const;

private:
  Eval_Task* rhs;
  Evaluator_Prefix_Operator* evaluator;
};


class Evaluator_Not : public Evaluator_Prefix_Operator
{
public:
  struct Statement_Maker : public Generic_Statement_Maker< Evaluator_Not >
  {
    virtual Statement* create_statement(const Token_Node_Ptr& tree_it,
        Statement::Factory& stmt_factory, Parsed_Query& global_settings, Error_Output* error_output);
    Statement_Maker() : Generic_Statement_Maker("eval-not") { Statement::maker_by_token()["!"].push_back(this); }
  };
  static Statement_Maker statement_maker;
      
  virtual std::string dump_xml(const std::string& indent) const
  { return indent + "<eval-not>\n" + rhs->dump_xml(indent + "  ") + indent + "</eval-not>\n"; }
  virtual std::string dump_compact_ql(const std::string&) const { return std::string("!") + rhs->dump_compact_ql(""); }

  Evaluator_Not(int line_number_, const map< string, string >& input_attributes,
                   Parsed_Query& global_settings);
  virtual string get_name() const { return "eval-not"; }
  virtual ~Evaluator_Not() {}
  
  virtual std::string process(const std::string& rhs_result) const;
};


class Evaluator_Negate : public Evaluator_Prefix_Operator
{
public:
  struct Statement_Maker : public Generic_Statement_Maker< Evaluator_Negate >
  {
    virtual Statement* create_statement(const Token_Node_Ptr& tree_it,
        Statement::Factory& stmt_factory, Parsed_Query& global_settings, Error_Output* error_output);
    Statement_Maker() : Generic_Statement_Maker("eval-negate") { Statement::maker_by_token()["-"].push_back(this); }
  };
  static Statement_Maker statement_maker;
      
  virtual std::string dump_xml(const std::string& indent) const
  { return indent + "<eval-negate>\n" + rhs->dump_xml(indent + "  ") + indent + "</eval-negate>\n"; }
  virtual std::string dump_compact_ql(const std::string&) const { return std::string("-") + rhs->dump_compact_ql(""); }
      
  Evaluator_Negate(int line_number_, const map< string, string >& input_attributes,
                   Parsed_Query& global_settings);
  virtual string get_name() const { return "eval-negate"; }
  virtual ~Evaluator_Negate() {}
  
  virtual std::string process(const std::string& rhs_result) const;
};


class Evaluator_Number : public Evaluator_Prefix_Operator
{
public:
  struct Statement_Maker : public Generic_Statement_Maker< Evaluator_Number >
  {
    virtual Statement* create_statement(const Token_Node_Ptr& tree_it,
        Statement::Factory& stmt_factory, Parsed_Query& global_settings, Error_Output* error_output);
    Statement_Maker() : Generic_Statement_Maker("eval-number")
    { Statement::maker_by_func_name()["number"].push_back(this); }
  };
  static Statement_Maker statement_maker;
      
  virtual std::string dump_xml(const std::string& indent) const
  { return indent + "<eval-number>\n" + (rhs ? rhs->dump_xml(indent + "  ") : "") + indent + "</eval-number>\n"; }
  virtual std::string dump_compact_ql(const std::string&) const
  { return std::string("number(\"") + (rhs ? rhs->dump_compact_ql("") : "") + "\")"; }

  Evaluator_Number(int line_number_, const map< string, string >& input_attributes,
                   Parsed_Query& global_settings);
  virtual string get_name() const { return "eval-number"; }
  virtual ~Evaluator_Number() {}
  
  virtual std::string process(const std::string& rhs_result) const;
};


class Evaluator_Is_Num : public Evaluator_Prefix_Operator
{
public:
  struct Statement_Maker : public Generic_Statement_Maker< Evaluator_Is_Num >
  {
    virtual Statement* create_statement(const Token_Node_Ptr& tree_it,
        Statement::Factory& stmt_factory, Parsed_Query& global_settings, Error_Output* error_output);
    Statement_Maker() : Generic_Statement_Maker("eval-is-num")
    { Statement::maker_by_func_name()["is_num"].push_back(this); }
  };
  static Statement_Maker statement_maker;
      
  virtual std::string dump_xml(const std::string& indent) const
  { return indent + "<eval-is-num>\n" + (rhs ? rhs->dump_xml(indent + "  ") : "") + indent + "</eval-is-num>\n"; }
  virtual std::string dump_compact_ql(const std::string&) const
  { return std::string("is_num(\"") + (rhs ? rhs->dump_compact_ql("") : "") + "\")"; }

  Evaluator_Is_Num(int line_number_, const map< string, string >& input_attributes,
                   Parsed_Query& global_settings);
  virtual string get_name() const { return "eval-is-num"; }
  virtual ~Evaluator_Is_Num() {}
  
  virtual std::string process(const std::string& rhs_result) const;
};


#endif
