#include "stdafx.h"
#include "sceneManager.h"
#include "cGameNode.h"

cGameNode* sceneManager::_currentScene = NULL;
cGameNode* sceneManager::_loadingScene = NULL;
cGameNode* sceneManager::_readyScene = NULL;

sceneManager::sceneManager()
{
}


sceneManager::~sceneManager()
{
}

HRESULT sceneManager::init()
{
	_currentScene = NULL;
	_readyScene = NULL;
	_loadingScene = NULL;

	return S_OK;
}

void sceneManager::release()
{
	mapSceneIter miSceneList = _mSceneList.begin();

	for ( ; miSceneList != _mSceneList.end(); )
	{
		//지워지면 반복자를 증가시키지 않는다
		if ( miSceneList->second != NULL )
		{
			if ( miSceneList->second == _currentScene ) miSceneList->second->Release();
			SAFE_DELETE( miSceneList->second );

			miSceneList = _mSceneList.erase( miSceneList );
		}
		else
		{
			++miSceneList;
		}
	}

	_mSceneList.clear();


	//로딩 씬 릴리즈
	miSceneList = _mLoadingSceneList.begin();

	for ( ; miSceneList != _mLoadingSceneList.end(); )
	{
		//지워지면 반복자를 증가시키지 않는다
		if ( miSceneList->second != NULL )
		{
			if ( miSceneList->second == _loadingScene ) miSceneList->second->Release();
			SAFE_DELETE( miSceneList->second );

			miSceneList = _mLoadingSceneList.erase( miSceneList );
		}
		else
		{
			++miSceneList;
		}
	}

	_mLoadingSceneList.clear();
}

void sceneManager::update()
{
	if ( _currentScene ) _currentScene->update();
}

void sceneManager::render()
{
	if ( _currentScene ) _currentScene->render();
}

void sceneManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (_currentScene) _currentScene->WndProc(hWnd, message, wParam, lParam);
}

cGameNode * sceneManager::addScene( string sceneName, cGameNode * scene )
{
	if ( !scene ) return NULL;

	_mSceneList.insert( make_pair( sceneName, scene ) );

	return scene;
}

cGameNode * sceneManager::addLoadingScene( string loadingSceneName, cGameNode * scene )
{
	if ( !scene ) return NULL;

	_mLoadingSceneList.insert( make_pair( loadingSceneName, scene ) );

	return scene;
}

HRESULT sceneManager::changeScene( string sceneName )
{
	//바꾸려하는 씬을 찾는다
	mapSceneIter find = _mSceneList.find( sceneName );

	//바꾸지 못하면 펄스 리턴
	if ( find == _mSceneList.end() ) return E_FAIL;

	//바꾸려 하는 씬과 현재씬이 같으면 그냥 리턴
	if ( find->second == _currentScene ) return S_OK;

	//새롭게 변경되는 씬을 초기화
	if ( SUCCEEDED( find->second->init() ) )
	{
		//기존에 씬이 있으면 릴리즈
		if ( _currentScene ) _currentScene->Release();
		_currentScene = find->second;

		return S_OK;
	}

	return E_FAIL;
}

HRESULT sceneManager::changeScene( string sceneName, string loadingSceneName )
{
	//바꾸려 하는 씬을 찾는다
	mapSceneIter find = _mSceneList.find( sceneName );

	//바꾸지 못하면 펄스 리턴
	if ( find == _mSceneList.end() ) return E_FAIL;

	//바꾸려 하는 씬과 현재씬이 같으면
	if ( find->second == _currentScene ) return S_OK;

	//바꾸려 하는 씬을 찾는다
	mapSceneIter findLoading = _mLoadingSceneList.find( loadingSceneName );

	//찾지 못했으면 일반 로딩시키자
	if ( findLoading == _mLoadingSceneList.end() ) return changeScene( loadingSceneName );

	//로딩 씬 초기화
	if ( SUCCEEDED( findLoading->second->init() ) )
	{
		//씬이 있다면 릴리즈
		if ( _currentScene ) _currentScene->Release();

		//로딩씬
		_loadingScene = findLoading->second;

		//로딩 끝난 후 변경될 씬
		_readyScene = find->second;

		//로딩씬 호출
		CloseHandle( CreateThread( NULL, 0, loadingThread, NULL, 0, &_loadThreadID ) );
	}

}


DWORD CALLBACK loadingThread( LPVOID prc )
{
	//교체될 init 함수를 실행한다
	sceneManager::_readyScene->init();

	//현재 씬을 교체될 씬으로 바꾼다.
	sceneManager::_currentScene = sceneManager::_readyScene;

	//로딩씬을 해제한다
	sceneManager::_loadingScene->Release();
	sceneManager::_loadingScene = NULL;
	sceneManager::_readyScene = NULL;

	return 0;
}
