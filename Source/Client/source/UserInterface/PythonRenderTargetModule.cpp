#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "PythonApplication.h"

PyObject *renderTargetSelectModel(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int modelIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SelectModel(modelIndex);
	return Py_BuildNone();
}

PyObject *renderTargetSetArmor(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->ChangeArmor(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject *renderTargetSetWeapon(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->ChangeWeapon(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject *renderTargetSetHair(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();

	bool isValue = false;

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		PyTuple_GetBoolean(poArgs, 2, &isValue);

		if (!isValue)
		{
			CItemData* pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(iItemVnum, &pItemData))
				return Py_BuildNone();
			iItemVnum = pItemData->GetValue(3);
		}

		if (target->IsReady())
			target->GetModel()->SetHair(iItemVnum);
	}
	return Py_BuildNone();
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
PyObject *renderTargetSetAcce(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->SetAcce(iItemVnum);
	}
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
PyObject *renderTargetSetAura(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->SetAura(iItemVnum);
	}
	return Py_BuildNone();
}
#endif

PyObject *renderTargetSetMount(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->MountHorse(iItemVnum);
	}
	return Py_BuildNone();
}

PyObject *renderTargetSetRotation(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	bool bRot;
	if (!PyTuple_GetBoolean(poArgs, 1, &bRot))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetRotation(bRot);
	return Py_BuildNone();
}

PyObject *renderTargetSetVisibility(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	bool isShow;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetVisibility(isShow);
	return Py_BuildNone();
}

PyObject *renderTargetSetBackground(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	char *szPathName;
	if (!PyTuple_GetString(poArgs, 1, &szPathName))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->CreateBackground(szPathName, CPythonApplication::Instance().GetWidth(), CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject *renderTargetResetModel(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->ResetModel();
	return Py_BuildNone();
}

PyObject *renderTargetDestroy(PyObject *poSelf, PyObject *poArgs)
{
	CRenderTargetManager::Instance().Destroy();
	return Py_BuildNone();
}

PyObject *renderSetModelV3Eye(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetModelV3Eye(x, y, z);
	return Py_BuildNone();
}

PyObject *renderSetModelV3Target(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float x, y, z;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();
	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetModelV3Target(x, y, z);
	return Py_BuildNone();
}

PyObject *renderGetFreeIndex(PyObject *poSelf, PyObject *poArgs)
{
	int iMin;
	if (!PyTuple_GetInteger(poArgs, 0, &iMin))
		return Py_BadArgument();
	int iMax;
	if (!PyTuple_GetInteger(poArgs, 1, &iMax))
		return Py_BadArgument();
	for (int j = iMin; j < iMax; ++j)
	{
		if (!CRenderTargetManager::Instance().GetRenderTarget(j))
			return Py_BuildValue("i", j);
	}
	return Py_BuildValue("i", iMin + 20);
}

PyObject *renderSetModelRender(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	bool bStatus;
	if (!PyTuple_GetBoolean(poArgs, 1, &bStatus))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetModelStatus(bStatus);
	return Py_BuildNone();
}

PyObject *renderAddAffect(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iEffectIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iEffectIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->AttachWikiAffect(iEffectIndex);
	}
	return Py_BuildNone();
}

PyObject *renderRemoveAffect(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	int iEffectIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iEffectIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->RemoveWikiAffect(iEffectIndex);
	}
	return Py_BuildNone();
}

PyObject *renderClearAffect(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->GetModel()->ClearWikiAffect();
	}
	return Py_BuildNone();
}

PyObject *renderZoomCamera(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 1, &iDirection))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
		{
			const float fRatio = 1.0f + CPythonApplication::Instance().GetCameraZoomSpeed() * float(iDirection);
			target->Zoom(fRatio);
		}
	}
	return Py_BuildNone();
}

PyObject *renderRotateEyeAroundTarget(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float fPitch;
	if (!PyTuple_GetFloat(poArgs, 1, &fPitch))
		return Py_BadArgument();

	float fRoll;
	if (!PyTuple_GetFloat(poArgs, 2, &fRoll))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
	{
		if (target->IsReady())
			target->RotateEyeAroundTarget(fPitch, fRoll);
	}
	return Py_BuildNone();
}

PyObject *renderSetScale(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	float fScale;
	if (!PyTuple_GetFloat(poArgs, 1, &fScale))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	if (target)
		target->SetScale(fScale);
	return Py_BuildNone();
}

PyObject *renderIsShow(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);
	return Py_BuildValue("i", target ? target->IsShow() : 0);
}

PyObject *renderSetZoom(PyObject *poSelf, PyObject *poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	bool bZoom = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &bZoom))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetZoom(bZoom);
	return Py_BuildNone();
}

void initRenderTarget()
{
	static PyMethodDef s_methods[] =
	{
		{"SelectModel", renderTargetSelectModel, METH_VARARGS},
		{"SetVisibility", renderTargetSetVisibility, METH_VARARGS},
		{"SetBackground", renderTargetSetBackground, METH_VARARGS},
		{"SetArmor", renderTargetSetArmor, METH_VARARGS},
		{"SetWeapon", renderTargetSetWeapon, METH_VARARGS},
		{"SetHair", renderTargetSetHair, METH_VARARGS},
		{"SetRotation", renderTargetSetRotation, METH_VARARGS},
		{"SetAcce", renderTargetSetAcce, METH_VARARGS},
		{"SetAura", renderTargetSetAura, METH_VARARGS},
		{"SetMount", renderTargetSetMount, METH_VARARGS},
		{"ResetModel", renderTargetResetModel, METH_VARARGS},
		{"Destroy", renderTargetDestroy, METH_VARARGS},
		{"SetModelV3Eye", renderSetModelV3Eye, METH_VARARGS },
		{"SetModelV3Target", renderSetModelV3Target, METH_VARARGS },
		{"GetFreeIndex", renderGetFreeIndex, METH_VARARGS },

		{"SetModelRender", renderSetModelRender, METH_VARARGS },
		{"AddAffect", renderAddAffect, METH_VARARGS },
		{"RemoveAffect", renderRemoveAffect, METH_VARARGS },
		{"ClearAffect", renderClearAffect, METH_VARARGS },

		{"IsShow", renderIsShow, METH_VARARGS },
		{"Zoom", renderZoomCamera, METH_VARARGS },
		{"SetZoom", renderSetZoom, METH_VARARGS },
		{"SetScale", renderSetScale, METH_VARARGS },
		{"RotateEyeAroundTarget", renderRotateEyeAroundTarget, METH_VARARGS },

		{nullptr, nullptr, 0},
	};

	PyObject *poModule = Py_InitModule("renderTarget", s_methods);
}
#endif
