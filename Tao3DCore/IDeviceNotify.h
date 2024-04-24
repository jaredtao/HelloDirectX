#pragma once

class IDeviceNotify
{
public:
	virtual ~IDeviceNotify() { }

	virtual void onDeviceLost()	   = 0;
	virtual void onDeviceRestore() = 0;

	void setLost(bool v)
	{
		m_losted = v;
	}
	bool lost() const
	{
		return m_losted;
	}

protected:
	bool m_losted = false;
};