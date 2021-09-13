import React,{useState} from 'react';
import { Button,Image ,Table} from 'antd';
import ProList from '@ant-design/pro-list';
import { DownloadOutlined } from '@ant-design/icons';
const App =function (){ 
  const [loading,setLoading]=useState(true);
  const currentDir=GetQueryString(window.location.search,"dir");
  const [dataSource,setDataSource]=useState([]);
  function GetQueryString(paraPart:string,name:string) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = paraPart.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return "";
}

  if (loading)
  {
    fetch('/api/dir?dir='+currentDir,{
      method:'GET'
    }).then((response)=>{
      return response.json();
    }).then((data)=>{
      for (var k of data)
        k.image=k.type===4?"folder.jpg":"file.jpg"
      setLoading(false);
      setDataSource(data);
    })
  }
  return (
    <ProList<any>
    onRow={(record: any) => {
      return {
        onClick: () => {
          if (record.type===4)
          {
            window.location.href="/index.html?dir="+currentDir+record.name+"/";
          }
          if (record.type===8)
          {
            window.location.href="/file?file="+currentDir+record.name;
          }
        },
      };
    }}
    rowKey="name"
    headerTitle="文件列表"
    loading={loading}
    dataSource={dataSource}
    showActions="hover"
    showExtra="hover"
    metas={{
      title: {
        dataIndex: 'name',
      },
      avatar: {
        dataIndex: 'image',
      },
      actions: {
        render: (text, row) => [
          <Button type="primary" shape="round" icon={<DownloadOutlined />} onClick={()=>{
            if (row.type===4)
            {
              window.location.href="/index.html?dir="+currentDir+row.name+"/";
            }
            if (row.type===8)
            {
              window.location.href="/file?file="+currentDir+row.name;
            }
            }} />
        ],
        search: false,
      },
      }}
      
      />
);
  }
export default App;
