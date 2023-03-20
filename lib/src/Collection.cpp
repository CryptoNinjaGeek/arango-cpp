//
// Created by Carsten Tang on 19/03/2023.
//

#include <zutano/Collection.h>
#include <zutano/Connection.h>
#include <zutano/Database.h>
#include <zutano/Exceptions.h>
#include <zutano/Tools.h>

namespace zutano {

class CollectionPimpl : public PrivateImpl {
 public:
  Connection connection_;
  Database db_;
  std::string name_;

  static inline auto Pimpl(std::shared_ptr<PrivateImpl> p) {
	return std::dynamic_pointer_cast<CollectionPimpl>(p);
  }
};

Collection::Collection(Connection conn, Database db, std::string name) {
  auto p = std::make_shared<CollectionPimpl>();
  p->connection_ = conn;
  p->name_ = name;
  p->db_ = db;
  p_ = p;
}

auto Collection::Insert(nlohmann::json doc, InsertInput input) -> nlohmann::json {
  auto p = CollectionPimpl::Pimpl(p_);
  std::vector<StringPair> params;

  params.push_back(StringPair("returnNew", tools::to_string(input.return_new)));
  params.push_back(StringPair("silent", tools::to_string(input.silent)));
  params.push_back(StringPair("overwrite", tools::to_string(input.overwrite)));
  params.push_back(StringPair("returnOld", tools::to_string(input.return_old)));

  if (input.sync)
	params.push_back(StringPair("waitForSync", tools::to_string(input.sync.value())));
  if (input.overwrite_mode)
	params.push_back(StringPair("overwriteMode", input.overwrite_mode.value()));
  if (input.keep_none)
	params.push_back(StringPair("keepNull", tools::to_string(input.keep_none.value())));
  if (input.merge)
	params.push_back(StringPair("mergeObjects", tools::to_string(input.merge.value())));

  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->db_.name())
	  .Collection(p->name_)
	  .Parameters(params)
	  .Endpoint("/document")
	  .Data(doc.dump(2));

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

auto Collection::AddHashIndex(HashIndexCreateInput input) -> nlohmann::json {
  auto p = CollectionPimpl::Pimpl(p_);

  nlohmann::json data = {
	  {"type", "hash"},
	  {"fields", input.fields},
  };

  if (input.unique)
	data["unique"] = input.unique.value();
  if (input.sparse)
	data["sparse"] = input.sparse.value();
  if (input.deduplicate)
	data["deduplicate"] = input.deduplicate.value();
  if (input.name)
	data["name"] = input.name.value();
  if (input.in_background)
	data["inBackground"] = input.in_background.value();

  return AddIndex(data);
}

auto Collection::AddIndex(nlohmann::json data) -> nlohmann::json {
  auto p = CollectionPimpl::Pimpl(p_);
  auto r = Request()
	  .Method(HttpMethod::POST)
	  .Database(p->db_.name())
	  .Parameters({StringPair("collection", p->name_)})
	  .Endpoint("/index")
	  .Data(data.dump(2));

  auto response = p->connection_.SendRequest(r);
  if (response.contains({401, 403}))
	throw AuthenticationError();
  else if (not response.is_success())
	throw ServerError(response.error_message(), response.error_code());

  return response.body();
}

} // zutano