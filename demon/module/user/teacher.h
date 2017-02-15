#ifndef SHARED_USER_TEACHER_H__
#define SHARED_USER_TEACHER_H__

struct school
{
	std::string id;
	std::string name;
};


class Teacher
{
public:
	SINGLETON_DEFINE(Teacher);

	void SetName(const std::string &name) {
		name_ = name;
	}

	void SetId(const std::string &id) {
		id_ = id;
	}

	void SetSchools(const std::vector<school> &schools) {
		schools_ = schools;
	}

	std::string GetId() {
		return id_;
	}

	std::string GetName() {
		return name_;
	}
	
private:
	Teacher() {}

private:
	std::string name_;
	std::string id_;
	std::vector<school> schools_;
};


#endif // ~SHARED_USER_TEACHER_H__