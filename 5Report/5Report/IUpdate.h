#ifndef IUPDATE_H
#define IUPDATE_H
class IUpdate
{
public:
	virtual void Update(IUpdate* iupdate) = 0;

};
#endif //!IUPDATE_H