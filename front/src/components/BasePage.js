import React, { Component } from 'react';
import { Layout, Input, Menu } from 'antd';

export default class BasePage extends Component {
    page() { return <div></div> }

    render() {
        return <Layout>
            <Layout.Header>
                <a
                    style={{ color: '#ffffff', fontSize: '27px', float: 'left', marginRight: '28px' }}
                    onClick={ e => this.props.history.push('/') }
                >
                    Gitter Kid
                </a>

                <div
                    style={{ float: 'left', marginRight: '28px' }}
                >
                    <Input.Search
                        size="large"
                        style={{ width: '360px' }}
                        placeholder="Search Gitter Kid"
                        onSearch={
                            keyword => {
                                if (this.props.history.location.pathname !== '/query') {
                                    this.props.history.push(`/query/${keyword}`);
                                }
                                this.props.dispatch({
                                    type: 'repository/query',
                                    payload: { keyword }
                                });
                            }
                        }
                    />
                </div>

                <div
                    style={{ float: 'left', marginRight: '28px' }}
                >
                    <Menu
                        theme="dark"
                        mode="horizontal"
                        style={{ lineHeight: '64px' }}
                        onClick={
                            e => {
                                switch (e.key) {
                                    case 'repositories':
                                        this.props.history.push('/query');
                                        break;
                                    default: break;
                                }
                            }
                        }
                    >
                        <Menu.Item key="repositories">Repositories</Menu.Item>
                    </Menu>
                </div>
            </Layout.Header>
            <Layout.Content style={{ padding: '20px 50px', background: '#ffffff' }}>
                <div>
                    { this.page() }
                </div>
            </Layout.Content>
            <Layout.Footer style={{ textAlign: 'center', background: '#ffffff' }}>
            </Layout.Footer>
        </Layout>
    }
}