import React, { Component } from 'react';
import { Table, Icon, Breadcrumb, Card, Dropdown, Menu } from 'antd';
import HighLight from 'react-syntax-highlight';


class PathComponent extends Component {
    render() {
        let route = this.props.singleDisplayType === 'tree'
            ? this.props.fs.path.split('/')
            : this.props.file.path.split('/');
        let breadcrumb = [];
        for (let i = 0; i < route.length; i++) {
            if (route[i] === '') {
                continue;
            }
            let path = '';
            for (let j = 0; j <= i; j++) {
                if (route[j] === '') {
                    continue;
                }
                path += `/${route[j]}`;
            }

            breadcrumb.push({ title: route[i], path });
        }
        return <Breadcrumb style={{ padding: 16, fontSize: 18 }} separator="/">
            <Breadcrumb.Item>
                <a style={{ margin: '0 8px' }} onClick={ e => this.props.dispatch({
                    type: 'repository/tree',
                    payload: {
                        ...this.props.fs,
                        path: ''
                    }}) }>
                    <Icon style={{ fontSize: 18 }} type="home" />
                    <span style={{ marginLeft: 6 }}>&lt;{
                        this.props.singleDisplayType === 'tree'
                        ? this.props.fs.flag
                        : this.props.file.flag
                    }&gt;</span>
                </a>

                <Dropdown placement="bottomLeft" trigger={['click']} overlay={<Menu>
                    {
                        this.props.branches.map(branch => <Menu.Item key={ branch }>
                            <a onClick={e => this.props.dispatch({
                                type: 'repository/tree',
                                payload: {
                                    repository: this.props.match.params.repository,
                                    flag: branch,
                                    path: ''
                                }
                            })}>{ branch }</a>
                        </Menu.Item>)
                    }
                </Menu>}>
                    <Icon type="down" />
                </Dropdown>
            </Breadcrumb.Item>

            {
                breadcrumb.map((item, index) => index < breadcrumb.length - 1
                ? <Breadcrumb.Item key={ item.title }>
                    <a onClick={e => this.props.dispatch({
                        type: 'repository/tree',
                        payload: {
                            ...this.props.fs,
                            path: item.path
                        }
                    })}>
                        { item.title }
                    </a>
                </Breadcrumb.Item>
                : <Breadcrumb.Item key={ item.title }>
                    <b>{ item.title }</b>
                </Breadcrumb.Item>)
            }
        </Breadcrumb>
    }
}

class TreeComponent extends Component {
    render() {
        return <Table
            showHeader={ false }
            pagination={ false }
            columns={[{
                key: 'column',
                render: (text, record) => record.type === 40000
                    ? <a onClick={ e => this.props.dispatch({
                        type: 'repository/tree',
                        payload: {
                            ...this.props.fs,
                            path: `${this.props.fs.path}/${record.name}`
                        }
                    })}>
                        <b>
                            <Icon type="folder" />
                            <span style={{ paddingLeft: 8 }}>{ record.name }</span>
                        </b>
                    </a>
                    : <a onClick={ e => this.props.dispatch({
                        type: 'repository/blob',
                        payload: {
                            ...this.props.fs,
                            path: `${this.props.fs.path}/${record.name}`
                        }
                    })}>
                        <Icon type="file" />
                        <span style={{ paddingLeft: 8 }}>{ record.name }</span>
                    </a>
            }]}
            dataSource={
                this.props.fs.content.map(item => ({
                    ...item,
                    key: item.name
                }))
            }
        />
    }
}

class CodeComponent extends Component {
    transform() {
        let middleCutResult = this.props.file.path.split('.');
        if (middleCutResult.length > 1) {
            switch (middleCutResult[middleCutResult.length - 1]) {
                case 'cs': return 'csharp';
                case 'html': return 'html';
                default: return 'markdown';
            }
        }
        return 'markdown';
    }
    render() {
        let lines = 0;
        let contentCount = this.props.file.content.length;
        for (let i = 0; i < contentCount; i++) {
            if (this.props.file.content[i] === '\n')
            {
                lines += 1;
            }
        }

        return <div>
            <div style={{ float: 'left' }}>
                <ul style={{
                    listStyle: 'none',
                    textAlign: 'right',
                    padding: '0 5px',
                    borderRight: '1px solid #dedede'
                }}>
                {
                    (() => {
                        let result = [];
                        for(let index = 0; index <= lines; index++) { result.push(index + 1); }
                        return result;
                    })().map(item => <li key={ item }>{ item }</li>)
                }
                </ul>
            </div>
            <div style={{ float: 'left', marginLeft: 8 }}>
                <HighLight lang={ this.transform() } value={ this.props.file.content } />
            </div>
        </div>
    }
}

export default class CodeTab extends Component {
    render() {
        return <div>
            <PathComponent { ...this.props } />
            <Card style={{ margin: '0 32px' }}>
            {
                this.props.singleDisplayType === 'tree'
                ? <TreeComponent { ...this.props } />
                : <CodeComponent { ...this.props } />
            }
            </Card>
        </div>
    }
}